#!/usr/bin/python3

import argparse
import logging
import os
from packaging.version import Version
from pathlib import Path
import shutil
import subprocess
from subprocess import PIPE, Popen

from module.args import parse_args
from module.path import ProjectPaths
from module.prepare_source import prepare_source
from module.profile import resolve_profile

# A = x86_64-linux-gnu or x86_64-linux-musl
# B = {i686,x86_64}-w64-mingw32
# XYZ: build = X, host = Y, target = Z
from module.AAA import build_AAA_library, build_AAA_python
from module.AAB import build_AAB_compiler, build_AAB_library
from module.ABB import build_ABB_toolchain

def clean(config: argparse.Namespace, paths: ProjectPaths):
  if paths.build.exists():
    shutil.rmtree(paths.build)
  if not config.no_cross and paths.x_prefix.exists():
    shutil.rmtree(paths.x_prefix)
  if paths.mingw_prefix.exists():
    shutil.rmtree(paths.mingw_prefix)

def prepare_dirs(paths: ProjectPaths):
  paths.assets.mkdir(parents = True, exist_ok = True)
  paths.build.mkdir(parents = True, exist_ok = True)
  paths.dist.mkdir(parents = True, exist_ok = True)

def _package(root: Path | str, src: Path | str, dst: Path):
  tar = Popen(['bsdtar', '-C', root, '-c', src], stdout = PIPE)
  zstd = Popen([
    'zstd', '-f',
    '--zstd=strat=5,wlog=27,hlog=25,slog=6',
    '-o', dst,
  ], stdin = tar.stdout)
  tar.stdout.close()
  zstd.communicate()
  tar.wait()
  if tar.returncode != 0 or zstd.returncode != 0:
    raise Exception('bsdtar | zstd failed')

def package_cross(paths: ProjectPaths):
  _package(paths.x_prefix.parent, paths.x_prefix.name, paths.x_pkg)

def package_mingw(paths: ProjectPaths):
  _package(paths.mingw_prefix.parent, paths.mingw_prefix.name, paths.mingw_pkg)

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

  prepare_source(ver, paths)

  os.environ['PATH'] = f'{paths.x_prefix}/bin:{os.environ["PATH"]}'
  if not config.no_cross:
    build_AAA_library(ver, paths, config)
    build_AAA_python(ver, paths, config)
    build_AAB_compiler(ver, paths, config)
    build_AAB_library(ver, paths, config)
    package_cross(paths)

  build_ABB_toolchain(ver, paths, config)
  package_mingw(paths)

if __name__ == '__main__':
  main()
