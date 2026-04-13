#!/usr/bin/python3

import argparse
import logging
import os
import shutil
from pathlib import Path
import subprocess
from pprint import pprint
import sys
from typing import List

from module.args import parse_args
from module.path import ProjectPaths
from module.profile import BranchProfile, resolve_profile
from module.util import XMAKE_ARCH_MAP, ensure, overlayfs_ro, common_cross_layers

def clean(config: argparse.Namespace, paths: ProjectPaths):
  if paths.test_dir.exists():
    shutil.rmtree(paths.test_dir)
  if paths.layer_dir.exists():
    shutil.rmtree(paths.layer_dir)

def prepare_dirs(paths: ProjectPaths):
  shutil.copytree(
    paths.test_src_dir,
    paths.test_dir,
    ignore = shutil.ignore_patterns(
      '.cache',
      '.vscode',
      '.xmake',
      'build',
    ),
  )
  ensure(paths.layer_dir)

def extract(path: Path, arx: Path):
  subprocess.run([
    'bsdtar',
    '-C', path,
    '-xf', arx,
    '--no-same-owner',
  ], check = True)

def prepare_test_binary(ver: BranchProfile, paths: ProjectPaths):
  extract(paths.layer_dir.parent, paths.cross_pkg)

def test_corss_compiler(ver, paths: ProjectPaths, verbose: List[str]):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.xmake / 'usr/local',

    paths.layer_AAB.crt_target / 'usr/local',
    *common_cross_layers(paths),
  ]):
    subprocess.check_call([
      'xmake', 'f', *verbose,
      '-p', 'mingw', '-a', XMAKE_ARCH_MAP[ver.arch],
      '--mingw=/usr/local',
    ], cwd = paths.test_dir)
    subprocess.check_call(['xmake', 'b', *verbose], cwd = paths.test_dir)
    subprocess.check_call(['xmake', 'test', *verbose], cwd = paths.test_dir)

def test_cross_shared(ver, paths: ProjectPaths, verbose: List[str]):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.xmake / 'usr/local',

    paths.layer_AAB.crt_target / 'usr/local',
    paths.layer_AAB.crt_shared / 'usr/local',
    paths.layer_AAB.gcc_lib_shared / 'usr/local',
    paths.layer_AAB.mcfgthread_shared / 'usr/local',
    paths.layer_AAB.winpthreads_shared / 'usr/local',
    *common_cross_layers(paths),
  ]):
    subprocess.check_call([
      'xmake', 'f', *verbose,
      '--builddir=build-shared',
      '-p', 'mingw', '-a', XMAKE_ARCH_MAP[ver.arch],
      '--mingw=/usr/local',
    ], cwd = paths.test_dir)
    subprocess.check_call(['xmake', 'b', *verbose], cwd = paths.test_dir)
    subprocess.check_call(
      ['xmake', 'test', *verbose],
      cwd = paths.test_dir,
      env = {
        **os.environ,
        'WINEPATH': f'/usr/local/{ver.target}/bin',
      },
    )

def main():
  config = parse_args()

  if config.verbose >= 2:
    logging.basicConfig(level = logging.DEBUG)
    os.environ['WINEDEBUG'] = ''
    xmake_verbose = ['-vD']
  elif config.verbose >= 1:
    logging.basicConfig(level = logging.INFO)
    os.environ['WINEDEBUG'] = 'fixme-all'
    xmake_verbose = ['-v']
  else:
    logging.basicConfig(level = logging.ERROR)
    os.environ['WINEDEBUG'] = '-all'
    xmake_verbose = []

  ver = resolve_profile(config)
  paths = ProjectPaths(config, ver)

  clean(config, paths)

  prepare_dirs(paths)

  prepare_test_binary(ver, paths)

  test_report = {
    'fail': False,
  }

  # https://bugs.archlinux.org/task/78702
  os.environ['LIBMOUNT_FORCE_MOUNT2'] = 'always'
  os.environ['XMAKE_ROOT'] = 'y'

  try:
    test_corss_compiler(ver, paths, xmake_verbose)
    test_report['cross-static'] = 'okay'
  except Exception as e:
    test_report['fail'] = 'True'
    test_report['cross-static'] = repr(e)

  try:
    test_cross_shared(ver, paths, xmake_verbose)
    test_report['cross-shared'] = 'okay'
  except Exception as e:
    test_report['fail'] = 'True'
    test_report['cross-shared'] = repr(e)

  print("============================== XTEST REPORT ==============================")
  pprint(test_report)

  if test_report['fail']:
    sys.exit(1)

if __name__ == '__main__':
  main()
