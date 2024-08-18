import logging
from pathlib import Path
import subprocess
from typing import List

from module.profile import ProfileInfo

def cflags_build(
  suffix: str = '',
  common_extra: List[str] = [],
  ld_extra: List[str] = [],
  c_extra: List[str] = [],
  cxx_extra: List[str] = [],
) -> List[str]:
  common = ['-Os']
  ld = ['-s']
  return [
    f'CFLAGS{suffix}=' + ' '.join(common + common_extra + c_extra),
    f'CXXFLAGS{suffix}=' + ' '.join(common + common_extra + cxx_extra),
    f'LDFLAGS{suffix}=' + ' '.join(ld + ld_extra),
  ]

def cflags_host(
  suffix: str = '',
  common_extra: List[str] = [],
  ld_extra: List[str] = [],
  c_extra: List[str] = [],
  cxx_extra: List[str] = [],
) -> List[str]:
  common = ['-Os']
  ld = ['-s']
  return [
    f'CFLAGS{suffix}=' + ' '.join(common + common_extra + c_extra),
    f'CXXFLAGS{suffix}=' + ' '.join(common + common_extra + cxx_extra),
    f'LDFLAGS{suffix}=' + ' '.join(ld + ld_extra),
  ]

def cflags_target(
  suffix: str = '',
  common_extra: List[str] = [],
  ld_extra: List[str] = [],
  c_extra: List[str] = [],
  cxx_extra: List[str] = [],
) -> List[str]:
  common = ['-Os']
  ld = ['-s']
  return [
    f'CFLAGS{suffix}=' + ' '.join(common + common_extra + c_extra),
    f'CXXFLAGS{suffix}=' + ' '.join(common + common_extra + cxx_extra),
    f'LDFLAGS{suffix}=' + ' '.join(ld + ld_extra),
  ]

def configure(component: str, cwd: Path, args: List[str]):
  res = subprocess.run(
    ['../configure', *args],
    cwd = cwd,
  )
  if res.returncode != 0:
    message = f'Build fail: {component} configure returned {res.returncode}'
    logging.critical(message)
    raise Exception(message)

def ensure(path: Path):
  path.mkdir(parents = True, exist_ok = True)

def make_custom(component: str, cwd: Path, extra_args: List[str], jobs: int):
  res = subprocess.run(
    ['make', *extra_args, f'-j{jobs}'],
    cwd = cwd,
  )
  if res.returncode != 0:
    message = f'Build fail: {component} make returned {res.returncode}'
    logging.critical(message)
    raise Exception(message)

def make_default(component: str, cwd: Path, jobs: int):
  make_custom(component + ' (default)', cwd, ['MAKEINFO=true'], jobs)

def make_install(component: str, cwd: Path):
  make_custom(component + ' (install)', cwd, ['MAKEINFO=true', 'install'], jobs = 1)
