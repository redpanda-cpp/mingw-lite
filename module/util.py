from contextlib import contextmanager
import logging
from pathlib import Path
import re
import shutil
import subprocess
from tempfile import TemporaryDirectory
from typing import Iterable, List, Union

from module.profile import ProfileInfo

XMAKE_ARCH_MAP = {
  '32': 'i386',
  '64': 'x86_64',
  'arm64': 'aarch64',
}

def add_objects_to_static_lib(ar: str, lib: Path, objects: Iterable[Path]):
  subprocess.run(
    [ar, 'r', lib, *objects],
    check = True,
  )

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

def configure(cwd: Path, args: List[str]):
  subprocess.run(
    ['../configure', *args],
    cwd = cwd,
    check = True,
  )

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

def make_custom(cwd: Path, extra_args: List[str], jobs: int):
  subprocess.run(
    ['make', *extra_args, f'-j{jobs}'],
    cwd = cwd,
    check = True,
  )

def make_default(cwd: Path, jobs: int):
  make_custom(cwd, [], jobs)

def make_destdir_install(cwd: Path, destdir: Path):
  make_custom(cwd, [f'DESTDIR={destdir}', 'install'], jobs = 1)

def make_install(cwd: Path):
  make_custom(cwd, ['install'], jobs = 1)

def meson_build(
  cwd: Path,
  jobs: int,
  targets: List[str] = [],
  build_dir: str = 'build',
):
  subprocess.run(
    ['meson', 'compile', '-C', build_dir, f'-j{jobs}', *targets],
    cwd = cwd,
    check = True,
  )

def meson_config(
  cwd: Path,
  extra_args: List[str],
  build_dir: str = 'build',
):
  subprocess.run(
    ['meson', 'setup', *extra_args, build_dir],
    cwd = cwd,
    check = True
  )

def meson_flags_B(
  cpp_extra: List[str] = [],
  common_extra: List[str] = [],
  ld_extra: List[str] = [],
  c_extra: List[str] = [],
  cxx_extra: List[str] = [],
  optimize_for_size: bool = False,
) -> List[str]:
  cpp = ['-DNDEBUG']
  common = ['-pipe']
  if optimize_for_size:
    build_type = 'minsize'
  else:
    build_type = 'release'
  return [
    '-Dc_args=' + ' '.join(cpp + cpp_extra + common + common_extra + c_extra),
    '-Dc_link_args=' + ' '.join(ld_extra),
    '-Dcpp_args=' + ' '.join(cpp + cpp_extra + common + common_extra + cxx_extra),
    '-Dcpp_link_args=' + ' '.join(ld_extra),
    f'--buildtype={build_type}',
    '--strip',
  ]

def meson_install(
  cwd: Path,
  destdir: Path,
  build_dir: str = 'build',
):
  subprocess.run(
    ['meson', 'install', '-C', build_dir, '--destdir', destdir],
    cwd = cwd,
    check = True,
  )

@contextmanager
def overlayfs_ro(merged: Union[Path, str], lower: list[Path]):
  try:
    if len(lower) == 1:
      subprocess.run([
        'mount',
        '--bind',
        lower[0],
        merged,
        '-o', 'ro',
      ], check = True)
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
    subprocess.run(['umount', merged], check = False)

def remove_info_main_menu(prefix: Path):
  info_main_menu = prefix / 'share/info/dir'
  if info_main_menu.exists():
    info_main_menu.unlink()

@contextmanager
def temporary_rw_overlay(path: Union[Path, str]):
  with TemporaryDirectory() as tmp:
    try:
      shutil.copytree(path, tmp, dirs_exist_ok = True)
      subprocess.run(['mount', '--bind', tmp, path], check = True)
      yield
    finally:
      subprocess.run(['umount', path], check = False)

def xmake_build(cwd: Path, jobs: int):
  subprocess.run(
    ['xmake', 'build', '-j', str(jobs)],
    cwd = cwd,
    check = True,
  )

def xmake_config(cwd: Path, extra_args: List[str]):
  subprocess.run(
    ['xmake', 'config', *extra_args],
    cwd = cwd,
    check = True,
  )

def xmake_install(cwd: Path, destdir: Path, targets: List[str] = []):
  subprocess.run(
    ['xmake', 'install', '-o', destdir, *targets],
    cwd = cwd,
    check = True,
  )
