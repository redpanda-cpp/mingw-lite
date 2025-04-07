import logging
from pathlib import Path
import re
import subprocess
from typing import List

from module.profile import ProfileInfo

def cflags_A(
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

def cflags_B(
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

def fix_libtool_absolute_reference(la_path: Path):
  with open(la_path, 'r') as f:
    lines = f.readlines()
  with open(la_path, 'w') as f:
    for line in lines:
      if line.startswith('dependency_libs='):
        libs_pattern = re.compile(r"dependency_libs='(.*)'")
        libs_value = re.search(libs_pattern, line).group(1)
        libs = libs_value.split()
        new_libs = []
        for lib in libs:
          if lib.startswith('/'):
            lib_name = Path(lib).stem
            if lib_name.startswith('lib'):
              lib_name = lib_name[3:]
            new_libs.append('-l' + lib_name)
          else:
            new_libs.append(lib)
        f.write(f"dependency_libs='{' '.join(new_libs)}'\n")
      else:
        f.write(line)

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

def make_destdir_install(component: str, cwd: Path, destdir: Path):
  make_custom(component + ' (install)', cwd, [f'DESTDIR={destdir}', 'install'], jobs = 1)

def make_install(component: str, cwd: Path):
  make_custom(component + ' (install)', cwd, ['MAKEINFO=true', 'install'], jobs = 1)
