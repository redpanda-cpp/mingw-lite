#!/usr/bin/python3

import argparse
import logging
import os
from packaging.version import Version
from pathlib import Path
import shutil
import subprocess
from subprocess import PIPE, Popen
from tempfile import NamedTemporaryFile

from module.args import parse_args
from module.path import ProjectPaths
from module.prepare_source import prepare_source
from module.profile import resolve_profile
from module.util import ensure, overlayfs_ro

# A = x86_64-linux-gnu or x86_64-linux-musl
# B = {i686,x86_64}-w64-mingw32
# XYZ: build = X, host = Y, target = Z
from module.AAA import build_AAA_library, build_AAA_tool
from module.AAB import build_AAB_compiler, build_AAB_library
from module.ABB import build_ABB_toolchain, build_ABB_xmake

def clean(config: argparse.Namespace, paths: ProjectPaths):
  if paths.build_dir.exists():
    shutil.rmtree(paths.build_dir)
  if not config.no_cross and paths.layer_AAA.prefix.exists():
    shutil.rmtree(paths.layer_AAA.prefix)
  if not config.no_cross and paths.layer_AAB.prefix.exists():
    shutil.rmtree(paths.layer_AAB.prefix)
  if paths.layer_ABB.prefix.exists():
    shutil.rmtree(paths.layer_ABB.prefix)

def prepare_dirs(paths: ProjectPaths):
  paths.assets_dir.mkdir(parents = True, exist_ok = True)
  paths.build_dir.mkdir(parents = True, exist_ok = True)
  paths.dist_dir.mkdir(parents = True, exist_ok = True)

def _sort_tarball(root: Path, src: Path):
  files: map[str, list[str]] = {}
  for file in src.glob('**/*'):
    if not file.is_dir():
      dn = file.relative_to(root).parent
      fn = file.name
      if dn not in files:
        files[dn] = []
      files[dn].append(fn)

  result = []
  for dn in sorted(files.keys()):
    result.append(f'{dn}/')
    for fn in sorted(files[dn]):
      result.append(f'{dn}/{fn}')
  return result

def _package(root: Path, files: list[str], dst: Path):
  with NamedTemporaryFile(delete = False) as listfile:
    listname = listfile.name
    for fn in files:
      listfile.write(f'{fn}\n'.encode())

  tar = Popen([
    'bsdtar', '-c',
    '-C', root,
    '-T', listname, '-n',
    '--numeric-owner',
  ], stdout = PIPE)
  zstd = Popen([
    'zstd', '-f',
    '--zstd=strat=5,wlog=27,hlog=25,slog=6,ovlog=9',
    '-o', dst,
  ], stdin = tar.stdout)
  tar.stdout.close()
  zstd.communicate()
  tar.wait()
  if tar.returncode != 0 or zstd.returncode != 0:
    raise Exception('bsdtar | zstd failed')

  os.unlink(listname)

def package_cross(paths: ProjectPaths):
  files = [
    *_sort_tarball(paths.layer_dir.parent, paths.layer_AAA.prefix),
    *_sort_tarball(paths.layer_dir.parent, paths.layer_AAB.prefix),
  ]

  _package(paths.layer_dir.parent, files, paths.cross_pkg)

def package_layers(pkg_dir: Path, layers: list[Path], dst: Path):
  files = []
  file_to_package_map: map[str, str] = {}
  for layer in layers:
    sorted_part = _sort_tarball(layer, layer)
    files.extend(map(
      lambda fn: f'{pkg_dir.name}/{fn}',
      sorted_part
    ))

    # check file collisions
    for fn in sorted_part:
      if fn.endswith('/'):
        continue
      if fn in file_to_package_map:
        raise Exception(f'file collision: {fn} in {layer.name} and {file_to_package_map[fn]}')
      file_to_package_map[fn] = layer.name

  ensure(pkg_dir)
  with overlayfs_ro(pkg_dir, layers):
    _package(pkg_dir.parent, files, dst)

def package_mingw(paths: ProjectPaths):
  layers = [
    paths.layer_ABB.binutils,
    paths.layer_ABB.crt,
    paths.layer_ABB.gcc,
    paths.layer_ABB.gdb,
    paths.layer_ABB.headers,
    paths.layer_ABB.make,
    paths.layer_ABB.pkgconf,
  ]

  package_layers(paths.pkg_dir, layers, paths.mingw_pkg)

def package_xmake(paths: ProjectPaths):
  layers = [
    paths.layer_ABB.xmake,
  ]

  package_layers(paths.pkg_dir, layers, paths.xmake_pkg)

def main():
  config = parse_args()

  if config.verbose >= 2:
    logging.basicConfig(level = logging.DEBUG)
  elif config.verbose >= 1:
    logging.basicConfig(level = logging.INFO)
  else:
    logging.basicConfig(level = logging.ERROR)

  logging.info("building GCC %s for %s", config.branch, config.profile)

  ver = resolve_profile(config)
  paths = ProjectPaths(config, ver)

  if config.clean:
    clean(config, paths)

  prepare_dirs(paths)

  prepare_source(ver, paths, config.download_only)

  if config.download_only:
    return

  if not config.no_cross:
    build_AAA_library(ver, paths, config)
    build_AAA_tool(ver, paths, config)
    build_AAB_compiler(ver, paths, config)
    build_AAB_library(ver, paths, config)
    package_cross(paths)

  build_ABB_toolchain(ver, paths, config)
  package_mingw(paths)
  build_ABB_xmake(ver, paths, config)
  package_xmake(paths)

if __name__ == '__main__':
  main()
