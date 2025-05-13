#!/usr/bin/python3

import argparse
import logging
import os
from pathlib import Path
import platform
from pprint import pprint
import shutil
import socket
import subprocess
from subprocess import PIPE
import sys

from module.args import parse_args
from module.fetch import check_and_extract
from module.path import ProjectPaths
from module.profile import BranchProfile, resolve_profile
from module.util import XMAKE_ARCH_MAP, ensure

def clean(config: argparse.Namespace, paths: ProjectPaths):
  if paths.test_dir.exists():
    shutil.rmtree(paths.test_dir)

def prepare_dirs(paths: ProjectPaths):
  shutil.copytree(paths.test_src_dir, paths.test_dir)

def prepare_test_binary(ver: BranchProfile, paths: ProjectPaths):
  check_and_extract(paths.test_mingw_dir, paths.mingw_pkg)

  root = paths.layer_dir.parent
  ensure(root)
  subprocess.run([
    'bsdtar', '-x',
    '-C', root,
    '-f', paths.cross_pkg,
    paths.layer_ABB.xmake.relative_to(root),
  ], check = True)

  shutil.copytree(paths.layer_ABB.xmake, paths.test_mingw_dir, dirs_exist_ok = True)

def winepath(path: Path):
  if platform.system() == 'Windows':
    return str(path)
  else:
    return subprocess.check_output(['winepath', '-w', path]).decode().strip()

def available_port():
  with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind(('localhost', 0))
    return s.getsockname()[1]

def test_mingw_compiler(ver: BranchProfile, paths: ProjectPaths, verbose: list[str]):
  xmake = paths.test_mingw_dir / 'bin/xmake.exe'
  subprocess.check_call([
    xmake, 'f', *verbose,
    '-p', 'mingw', '-a', XMAKE_ARCH_MAP[ver.arch],
    f'--mingw={winepath(paths.test_mingw_dir)}',
  ], cwd = paths.test_dir)
  subprocess.check_call([xmake, 'b', *verbose], cwd = paths.test_dir)
  subprocess.check_call([xmake, 'test', *verbose], cwd = paths.test_dir)

def test_mingw_make_gdb(ver: BranchProfile, paths: ProjectPaths):
  bin_dir = paths.test_mingw_dir / 'bin'
  make_exe = bin_dir / 'mingw32-make.exe'
  gdb_exe = bin_dir / 'gdb.exe'
  gdbserver_exe = bin_dir / 'gdbserver.exe'

  build_dir = paths.test_dir / 'build' / 'mingw' / XMAKE_ARCH_MAP[ver.arch] / 'debug'
  inferior = build_dir / 'breakpoint.exe'
  in_gdb_inferior = winepath(inferior).replace('\\', '/')
  ensure(build_dir)

  # make
  if platform.system() == 'Windows':
    saved_path = os.environ['PATH']
    os.environ['PATH'] = str(bin_dir) + ':' + os.environ['PATH']
  else:
    os.environ['WINEPATH'] = winepath(bin_dir)

  subprocess.check_call([make_exe, f'DIR={build_dir}', 'SUFFIX=.exe'], cwd = paths.test_dir)

  if platform.system() == 'Windows':
    os.environ['PATH'] = saved_path
  else:
    del os.environ['WINEPATH']
  
  # gdb
  port = available_port()
  comm = f'localhost:{port}'

  gdb_command_file = paths.test_dir / 'gdb_command.txt'
  with open(gdb_command_file, 'wb') as f:
    content = (
      f'file {in_gdb_inferior}\n'  # old releases disconnect when retriving symbol from gdbserver
      f'target remote {comm}\n'
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
    f.write(content.encode())

  expected_output = [
    '$1 = 1',
    '$2 = 2',
    '$3 = 3',
    '$4 = 5',
  ]

  gdbserver = subprocess.Popen([gdbserver_exe, comm, winepath(inferior)], cwd = paths.test_dir)
  gdb = subprocess.Popen([gdb_exe, '--batch', f'--command={gdb_command_file}'], cwd = paths.test_dir, stdout = PIPE)
  gdb.wait(timeout = 10.0)
  if gdb.returncode != 0:
    raise Exception(f"gdb exited with code {gdb.returncode}")
  gdbserver.wait(timeout = 1.0)
  if gdbserver.returncode != 0:
    raise Exception(f"gdbserver exited with code {gdbserver.returncode}")

  gdb_output = gdb.stdout.read().decode()
  for line in expected_output:
    if line not in gdb_output:
      raise Exception(f"expected output line '{line}' not found in gdb output:\n{gdb_output}")

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

  logging.info("testing GCC %s", config.branch)

  ver = resolve_profile(config)
  paths = ProjectPaths(config, ver)

  clean(config, paths)

  prepare_dirs(paths)

  prepare_test_binary(ver, paths)

  test_report = {
    'fail': False,
  }

  try:
    test_mingw_compiler(ver, paths, xmake_verbose)
    test_report['mingw64-compiler'] = "okay"
  except Exception as e:
    test_report['fail'] = True
    test_report['mingw64-compiler'] = repr(e)
  try:
    test_mingw_make_gdb(ver, paths)
    test_report['mingw64-make-gdb'] = "okay"
  except Exception as e:
    test_report['fail'] = True
    test_report['mingw64-make-gdb'] = repr(e)

  print("============================== TEST REPORT ==============================")
  pprint(test_report)

  if test_report['fail']:
    sys.exit(1)

if __name__ == '__main__':
  main()
