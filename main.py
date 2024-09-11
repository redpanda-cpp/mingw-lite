#!/usr/bin/python3

import argparse
import logging
import os
from packaging.version import Version
from pathlib import Path
import shutil
import subprocess
from subprocess import PIPE, Popen

from module.cross_compiler import build_cross_compiler
from module.mingw_toolchain import build_mingw_toolchain
from module.path import ProjectPaths
from module.prepare_source import download_and_patch
from module.profile import get_full_profile

def parse_args() -> argparse.Namespace:
  parser = argparse.ArgumentParser()
  parser.add_argument(
    '-b', '--branch',
    type = str,
    choices = [
      # C++17 era
      '15', '14', '13', '12', '11',
      # C++14 era
      '10', '9', '8', '7', '6',
      # C++98 era
      '5','4.9', '4.8',
    ],
    required = True,
    help = 'GCC branch to build',
  )
  parser.add_argument(
    '-p', '--profile',
    type = str,
    choices = [
      '64-mcf', '64-ucrt', '64-msvcrt',
      '32-mcf', '32-ucrt', '32-msvcrt',
      '32-legacy',
    ],
    required = True,
    help = 'MinGW profile to build',
  )

  parser.add_argument(
    '-c', '--clean',
    action = 'store_true',
    help = 'Clean build directories',
  )
  parser.add_argument(
    '-j', '--jobs',
    type = int,
    default = os.cpu_count(),
  )
  parser.add_argument(
    '-nx', '--no-cross',
    action = 'store_true',
    help = 'Do not build cross toolchain',
  )
  parser.add_argument(
    '-v', '--verbose',
    action = 'count',
    default = 0,
    help = 'Increase verbosity (up to 2)',
  )

  result = parser.parse_args()
  if result.profile in ['64-mcf', '32-mcf'] and Version(result.branch).major < 13:
    raise Exception('mcf profile requires GCC 13 or later')
  if result.profile in ['64-ucrt', '32-ucrt'] and Version(result.branch).major < 8:
    raise Exception('ucrt profile requires GCC 8 or later')
  if result.profile == '32-legacy' and Version(result.branch).major < 14:
    raise Exception('32-legacy profile not backported yet')

  return result

def clean(config: argparse.Namespace, paths: ProjectPaths):
  if paths.build.exists():
    shutil.rmtree(paths.build)
  if not config.no_cross and paths.x_prefix.exists():
    shutil.rmtree(paths.x_prefix)
  if paths.prefix.exists():
    shutil.rmtree(paths.prefix)

def prepare_dirs(paths: ProjectPaths):
  paths.assets.mkdir(parents = True, exist_ok = True)
  paths.build.mkdir(parents = True, exist_ok = True)
  paths.dist.mkdir(parents = True, exist_ok = True)

def package(paths: ProjectPaths):
  tar = Popen(['bsdtar', '-c', paths.x_prefix], stdout = PIPE)
  zstd = Popen([
    'zstd', '-f',
    '--zstd=strat=5,wlog=27,hlog=25,slog=6',
    '-o', paths.x_arx,
  ], stdin = tar.stdout)
  tar.stdout.close()
  zstd.communicate()
  tar.wait()
  if tar.returncode != 0 or zstd.returncode != 0:
    raise Exception('bsdtar | zstd failed')

  subprocess.run([
    '7z', 'a', '-t7z',
    '-mf=BCJ2', '-mx9', '-ms=on', '-mqs', '-m0=LZMA:d=64m:fb273',
    paths.arx,
    paths.prefix.name,
  ], check = True, cwd = paths.prefix.parent)

def main():
  config = parse_args()

  if config.verbose >= 2:
    logging.basicConfig(level = logging.DEBUG)
  elif config.verbose >= 1:
    logging.basicConfig(level = logging.INFO)
  else:
    logging.basicConfig(level = logging.ERROR)

  logging.info("building GCC %s for %s", config.branch, config.profile)

  profile = get_full_profile(config)
  paths = ProjectPaths(config, profile.ver)

  if config.clean:
    clean(config, paths)

  prepare_dirs(paths)

  download_and_patch(profile.ver, paths, profile.info)

  os.environ['PATH'] = f'{paths.x_prefix}/bin:{os.environ['PATH']}'

  if not config.no_cross:
    build_cross_compiler(profile.ver, paths, profile.info, config)

  build_mingw_toolchain(profile.ver, paths, profile.info, config)

  package(paths)

if __name__ == '__main__':
  main()
