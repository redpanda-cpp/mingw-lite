#!/usr/bin/python3

import argparse
from pathlib import Path
import shutil
import subprocess

from module.args import parse_args
from module.path import ProjectPaths
from module.profile import BranchProfile, resolve_profile
from module.util import XMAKE_ARCH_MAP, ensure

def clean(config: argparse.Namespace, paths: ProjectPaths):
  if paths.sat_dir.exists():
    shutil.rmtree(paths.sat_dir)

def prepare_dirs(paths: ProjectPaths):
  ensure(paths.sat_dir.parent)
  shutil.copytree(
    paths.test_src_dir,
    paths.sat_dir,
    ignore = shutil.ignore_patterns(
      '.cache',
      '.vscode',
      '.xmake',
      'build',
    ),
  )

def extract(path: Path, arx: Path):
  subprocess.run([
    'bsdtar',
    '-C', path,
    '-xf', arx,
    '--no-same-owner',
  ], check = True)

def prepare_test_binary(ver: BranchProfile, paths: ProjectPaths):
  extract(paths.sat_dir, paths.test_driver_pkg)
  extract(paths.sat_dir, paths.mingw_pkg)
  extract(paths.sat_dir, paths.xmake_pkg)

def write_gdb_commands(ver: BranchProfile, paths: ProjectPaths):
  xmake_arch = XMAKE_ARCH_MAP[ver.arch]
  build_dir = f'build/mingw/{xmake_arch}/debug'
  inferior = f'{build_dir}/breakpoint.exe'

  with open(paths.sat_dir / 'gdb-command.txt', 'wb') as f:
    content = (
      f'file {inferior}\n'
      'set sysroot C:\n'
      f'target remote localhost:1234\n'
      'b 14\n'
      'b 19\n'
      'c\n'
      'p fib[i]\n'  # i = 2, fib[i] = 1
      'c\n'
      'p fib[i]\n'  # i = 3, fib[i] = 2
      'c\n'
      'p fib[i]\n'  # i = 4, fib[i] = 3
      'c\n'
      'p fib[i]\n'  # i = 5, fib[i] = 5
      'c\n'
      'p fib_vec\n'
      'c\n'
    )
    f.write(content.encode())

def main():
  config = parse_args()

  ver = resolve_profile(config)
  paths = ProjectPaths(config, ver)

  clean(config, paths)

  prepare_dirs(paths)

  prepare_test_binary(ver, paths)

  write_gdb_commands(ver, paths)

if __name__ == '__main__':
  main()
