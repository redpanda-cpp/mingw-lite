#!/usr/bin/python3

import os
import sys

sys.path.append(os.getcwd())

import argparse
import shutil

from module.args import parse_args
from module.fetch import check_and_extract
from module.path import ProjectPaths
from module.profile import BranchProfile, resolve_profile
from module.util import XMAKE_ARCH_MAP

def clean(config: argparse.Namespace, paths: ProjectPaths):
  if paths.test_archive.exists():
    shutil.rmtree(paths.test_archive)

def prepare_dirs(paths: ProjectPaths):
  shutil.copytree(paths.test_src, paths.test_archive)

def prepare_test_binary(ver: BranchProfile, paths: ProjectPaths):
  check_and_extract(paths.test_archive_mingw, paths.mingw_pkg)

def test_mingw_compiler_batch(ver: BranchProfile, paths: ProjectPaths):
  xmake = (paths.test_archive_mingw / 'bin' / 'xmake.exe').relative_to(paths.test_archive)
  mingw = paths.test_archive_mingw.relative_to(paths.test_archive)

  with open(paths.test_archive / 'test_mingw_compiler.bat', 'wb') as f:
    content = (
      '@echo off\n'
      f'{xmake} f -v -p mingw -a {XMAKE_ARCH_MAP[ver.arch]} --mingw={mingw}\n'
      f'{xmake} b -v\n'
      f'{xmake} test -v\n'
      'pause\n'
    )
    f.write(content.replace('/', '\\').replace('\n', '\r\n').encode())

def test_mingw_make_gdb_batch(ver: BranchProfile, paths: ProjectPaths):
  mingw = paths.test_archive_mingw.relative_to(paths.test_archive)

  build_dir = f'build/mingw/{XMAKE_ARCH_MAP[ver.arch]}/debug'
  inferior = f'{build_dir}/breakpoint.exe'

  with open(paths.test_archive / 'test_mingw_make_gdb.bat', 'wb') as f:
    content = (
      '@echo off\n'
      f'set PATH=%CD%/{mingw}/bin;%PATH%\n'
      f'mkdir {build_dir}\n'
      f'mingw32-make DIR={build_dir} SUFFIX=.exe\n'
      'echo Please start gdbserver...\n'
      'pause\n'
      'gdb < gdb_command.txt\n'
      'pause\n'
    )
    f.write(content.replace('/', '\\').replace('\n', '\r\n').encode())

  with open(paths.test_archive / 'test_mingw_make_gdb_run_gdbserver.bat', 'wb') as f:
    content = (
      '@echo off\n'
      f'set PATH=%CD%/{mingw}/bin;%PATH%\n'
      f'gdbserver localhost:1234 {inferior}\n'
      'pause\n'
    )
    f.write(content.replace('/', '\\').replace('\n', '\r\n').encode())

  with open(paths.test_archive / 'gdb_command.txt', 'wb') as f:
    content = (
      f'file {inferior}\n'
      f'target remote localhost:1234\n'
      'b 12\n'
      'c\n'
      'p fib[i]\n'  # i = 2, fib[i] = 1
      'c\n'
      'p fib[i]\n'  # i = 3, fib[i] = 2
      'c\n'
      'p fib[i]\n'  # i = 4, fib[i] = 3
      'c\n'
      'p fib[i]\n'  # i = 5, fib[i] = 5
      'c\n'
    )
    f.write(content.replace('/', '\\').replace('\n', '\r\n').encode())

def main():
  config = parse_args()

  ver = resolve_profile(config)
  paths = ProjectPaths(config, ver)

  clean(config, paths)

  prepare_dirs(paths)

  prepare_test_binary(ver, paths)

  test_mingw_compiler_batch(ver, paths)

  test_mingw_make_gdb_batch(ver, paths)

if __name__ == '__main__':
  main()
