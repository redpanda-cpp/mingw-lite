import argparse
import logging
import os
import subprocess
from subprocess import PIPE

from module.profile import BRANCHES, PROFILES

def get_gcc_triplet():
  try:
    result = subprocess.run(['gcc', '-dumpmachine'], stdout = PIPE, stderr = PIPE, check = True)
    return result.stdout.decode('utf-8').strip()
  except Exception as e:
    logging.error(f'Failed to get GCC triplet: {e}')
    return None

def parse_args() -> argparse.Namespace:
  parser = argparse.ArgumentParser()
  parser.add_argument(
    '-b', '--branch',
    type = str,
    choices = BRANCHES.keys(),
    required = True,
    help = 'GCC branch to build',
  )
  parser.add_argument(
    '-p', '--profile',
    type = str,
    choices = PROFILES.keys(),
    required = True,
    help = 'MinGW profile to build',
  )
  parser.add_argument(
    '-es', '--enable-shared',
    action = 'store_true',
    help = 'Build shared runtime libraries (will break ABI stability when targeting NT < 6.0)',
  )

  gcc_triplet = get_gcc_triplet()
  parser.add_argument(
    '--build',
    type = str,
    default = gcc_triplet,
    required = not gcc_triplet,
    help = 'Build system triplet',
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
    '--download-only',
    action = 'store_true',
    help = 'Download sources only',
  )
  parser.add_argument(
    '-v', '--verbose',
    action = 'count',
    default = 0,
    help = 'Increase verbosity (up to 2)',
  )

  result = parser.parse_args()
  return result
