from contextlib import contextmanager
import logging
from pathlib import Path
import re
import subprocess
from typing import Iterable, List

from module.profile import ProfileInfo

XMAKE_ARCH_MAP = {
  '32': 'i386',
  '64': 'x86_64',
  'arm64': 'aarch64',
}

def add_objects_to_static_lib(ar: str, lib: Path, objects: Iterable[Path]):
  res = subprocess.run([
    ar, 'r',
    lib,
    *objects,
  ])
  if res.returncode != 0:
    message = f'Build fail: {lib.name} ar returned {res.returncode}'
    logging.critical(message)
    raise Exception(message)

def cflags_A(
  suffix: str = '',
  cpp_extra: List[str] = [],
  common_extra: List[str] = [],
  ld_extra: List[str] = [],
  c_extra: List[str] = [],
  cxx_extra: List[str] = [],
) -> List[str]:
  cpp = ['-DNDEBUG']
  common = ['-O2', '-pipe']
  ld = ['-s']
  return [
    f'CPPFLAGS{suffix}=' + ' '.join(cpp + cpp_extra),
    f'CFLAGS{suffix}=' + ' '.join(common + common_extra + c_extra),
    f'CXXFLAGS{suffix}=' + ' '.join(common + common_extra + cxx_extra),
    f'LDFLAGS{suffix}=' + ' '.join(ld + ld_extra),
  ]

def cflags_B(
  suffix: str = '',
  cpp_extra: List[str] = [],
  common_extra: List[str] = [],
  ld_extra: List[str] = [],
  c_extra: List[str] = [],
  cxx_extra: List[str] = [],
  optimize_for_size: bool = False,
  lto: bool = False,
) -> List[str]:
  cpp = ['-DNDEBUG']
  common = ['-pipe']
  ld = ['-s']
  if lto:
    # lto does not work with -Os
    common.extend(['-O2', '-flto'])
    ld.extend(['-O2', '-flto'])
  else:
    if optimize_for_size:
      common.append('-Os')
    else:
      common.append('-O2')
  return [
    f'CPPFLAGS{suffix}=' + ' '.join(cpp + cpp_extra),
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
  make_custom(component + ' (default)', cwd, [], jobs)

def make_destdir_install(component: str, cwd: Path, destdir: Path):
  make_custom(component + ' (install)', cwd, [f'DESTDIR={destdir}', 'install'], jobs = 1)

def make_install(component: str, cwd: Path):
  make_custom(component + ' (install)', cwd, ['install'], jobs = 1)

@contextmanager
def overlayfs_ro(merged: Path | str, lower: list[Path]):
  try:
    if len(lower) == 1:
      subprocess.run([
        'mount',
        '--bind',
        lower[0],
        merged,
        '-o', 'ro',
      ])
    else:
      lowerdir = ':'.join(map(str, lower))
      subprocess.run([
        'mount',
        '-t', 'overlay',
        'none',
        merged,
        '-o', f'lowerdir={lowerdir}',
      ], check = True)
    yield
  finally:
    subprocess.run(['umount', merged])

def xmake_build(component: str, cwd: Path, jobs: int):
  res = subprocess.run(
    ['xmake', 'build', '-j', str(jobs)],
    cwd = cwd,
  )
  if res.returncode != 0:
    message = f'Build fail: {component} xmake build returned {res.returncode}'
    logging.critical(message)
    raise Exception(message)

def xmake_config(component: str, cwd: Path, extra_args: List[str]):
  res = subprocess.run(
    ['xmake', 'config', *extra_args],
    cwd = cwd,
  )
  if res.returncode != 0:
    message = f'Build fail: {component} xmake config returned {res.returncode}'
    logging.critical(message)
    raise Exception(message)

def xmake_install(component: str, cwd: Path, destdir: Path, targets: List[str] = []):
  res = subprocess.run(
    ['xmake', 'install', '-o', destdir, *targets],
    cwd = cwd,
  )
  if res.returncode != 0:
    message = f'Build fail: {component} xmake install returned {res.returncode}'
    logging.critical(message)
    raise Exception(message)
