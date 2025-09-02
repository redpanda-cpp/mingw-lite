#!/usr/bin/python3

import argparse
from pathlib import Path
import shutil
import subprocess

from module.args import parse_args
from module.path import ProjectPaths
from module.profile import BranchProfile, resolve_profile
from module.util import XMAKE_ARCH_MAP, overlayfs_ro

def clean(config: argparse.Namespace, paths: ProjectPaths):
  if paths.sat_dir.exists():
    shutil.rmtree(paths.sat_dir)

def prepare_dirs(paths: ProjectPaths):
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
    '-C', path.parent,
    '-xf', arx,
    '--no-same-owner',
  ], check = True)

def prepare_test_binary(ver: BranchProfile, paths: ProjectPaths):
  extract(paths.sat_mingw_dir, paths.mingw_pkg)
  extract(paths.sat_mingw_dir, paths.xmake_pkg)

def write_gdb_commands(ver: BranchProfile, paths: ProjectPaths):
  mingw_arch = XMAKE_ARCH_MAP[ver.arch]
  xmake_arch = f'build/mingw/{mingw_arch}/debug'
  inferior = f'{xmake_arch}/breakpoint.exe'

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

def compile_test_programs(ver: BranchProfile, paths: ProjectPaths):
  mingw_dir = paths.sat_mingw_dir.relative_to(paths.sat_dir)
  xmake_arch = XMAKE_ARCH_MAP[ver.arch]
  debug_build_dir = f'build/mingw/{xmake_arch}/debug'

  flags = [
    '-std=c11', '-O2', '-s',
    f'-DMINGW_DIR="{mingw_dir}"',
    f'-DXMAKE_ARCH="{xmake_arch}"',
    f'-DDEBUG_BUILD_DIR="{debug_build_dir}"',
  ]

  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',
  ]):
    gcc_exe = f'{ver.target}-gcc'
    common_c = paths.root_dir / 'support/sat/common.c'
    test_compiler_c = paths.root_dir / 'support/sat/test-compiler.c'
    test_make_gdb_c = paths.root_dir / 'support/sat/test-make-gdb.c'
    test_compiler_exe = paths.sat_dir / 'test-compiler.exe'
    test_make_gdb_exe = paths.sat_dir / 'test-make-gdb.exe'

    subprocess.run([
      gcc_exe,
      *flags,
      test_compiler_c, common_c,
      '-o', test_compiler_exe,
    ], check=True)

    subprocess.run([
      gcc_exe,
      *flags,
      test_make_gdb_c, common_c,
      '-o', test_make_gdb_exe,
    ], check=True)

def main():
  config = parse_args()

  ver = resolve_profile(config)
  paths = ProjectPaths(config, ver)

  clean(config, paths)

  prepare_dirs(paths)

  prepare_test_binary(ver, paths)

  write_gdb_commands(ver, paths)

  compile_test_programs(ver, paths)

if __name__ == '__main__':
  main()
