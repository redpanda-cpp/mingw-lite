from contextlib import contextmanager
import logging
import os
from pathlib import Path
import re
import shutil
import subprocess
from tempfile import TemporaryDirectory
from typing import Iterable, List, Optional, Sequence, Union

from module.path import ProjectPaths
from module.platform import is_genuine_linux, is_wsl1

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
  optimize_for_speed: bool = False,
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
    if optimize_for_speed:
      common.append('-O2')
    else:
      common.append('-Os')
  return [
    f'CPPFLAGS{suffix}=' + ' '.join(cpp + cpp_extra),
    f'CFLAGS{suffix}=' + ' '.join(common + common_extra + c_extra),
    f'CXXFLAGS{suffix}=' + ' '.join(common + common_extra + cxx_extra),
    f'LDFLAGS{suffix}=' + ' '.join(ld + ld_extra),
  ]

def cmake_build(
  cwd: Path,
  jobs: int,
  targets: List[str] = [],
  build_dir: str = 'build',
):
  subprocess.run(
    ['cmake', '--build', build_dir, '-j', str(jobs), *targets],
    cwd = cwd,
    check = True,
  )

def cmake_config(
  cwd: Path,
  extra_args: List[str],
  build_dir: str = 'build',
):
  subprocess.run(
    ['cmake', '-G', 'Ninja', '-S', '.', '-B', build_dir, *extra_args],
    cwd = cwd,
    check = True,
  )

def cmake_flags_A() -> List[str]:
  return [
    '-DCMAKE_BUILD_TYPE=Release',
  ]

def cmake_flags_B(
  optimize_for_speed: bool = False,
  lto: bool = False,
) -> List[str]:
  build_type = ''
  lto_flag = []

  if optimize_for_speed:
    build_type = 'Release'
  else:
    build_type = 'MinSizeRel'
  if lto:
    lto_flag = ['-DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON']

  return [
    f'-DCMAKE_BUILD_TYPE={build_type}',
    *lto_flag,
  ]

def cmake_install(
  cwd: Path,
  destdir: Path,
  targets: List[str] = [],
  build_dir: str = 'build',
):
  subprocess.run(
    ['cmake', '--install', build_dir, *targets],
    cwd = cwd,
    check = True,
    env = {
      **os.environ,
      'DESTDIR': destdir,
    },
  )

def common_cross_layers(paths: ProjectPaths):
  return [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.gcc_lib / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.mcfgthread / 'usr/local',
    paths.layer_AAB.winpthreads / 'usr/local',
  ]

def configure(cwd: Path, args: List[str]):
  subprocess.run(
    ['../configure', *args],
    cwd = cwd,
    check = True,
  )

@contextmanager
def dt_sidecar_dir(path: Union[Path, str]):
  os.environ['DT_SIDECAR_DIR'] = str(path)
  try:
    yield
  finally:
    del os.environ['DT_SIDECAR_DIR']

def ensure(path: Path):
  path.mkdir(parents = True, exist_ok = True)

def extract_shared_libs(
  base_prefix: Path,
  shared_prefix: Optional[Path],
  include: Sequence[Union[str, Path]] = [],
  exclude: Sequence[Union[str, Path]] = [],
):
  bin_dir = base_prefix / 'bin'
  lib_dir = base_prefix / 'lib'

  # part 1: normal (dll + dll.a + extra .a)
  all_files = [
    *bin_dir.glob('*.dll'),
    *lib_dir.glob('*.dll.a'),
    *map(lambda p: base_prefix / p, include),
  ]
  exclude_files = [*map(lambda p: base_prefix / p, exclude)]
  files = [f for f in all_files if f not in exclude_files]

  for file in files:
    if shared_prefix:
      rel_dir = file.parent.relative_to(base_prefix)
      ensure(shared_prefix / rel_dir)
      shutil.move(file, shared_prefix / rel_dir / file.name)
    else:
      file.unlink()

  dll_in_lib = [*lib_dir.glob('*.dll')]
  for dll in dll_in_lib:
    if shared_prefix:
      ensure(shared_prefix / 'bin')
      shutil.move(dll, shared_prefix / 'bin' / dll.name)
    else:
      dll.unlink()

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
  optimize_for_speed: bool = False,
) -> List[str]:
  cpp = ['-DNDEBUG']
  common = ['-pipe']
  if optimize_for_speed:
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
def overlayfs_ro(merged: Union[Path, str], lower: Sequence[Union[Path, str]]):
  if type(merged) is not Path:
    merged = Path(merged)
  ensure(merged)
  if is_genuine_linux():
    # `mount -t overlay`, or `mount --bind` (if only 1 lower).
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
    try:
      yield
    finally:
      subprocess.run(['umount', merged], check = False)
  elif is_wsl1():
    # `mount -t tmpfs`, then copy.
    subprocess.run(['mount', '-t', 'tmpfs', 'none', merged], check = True)
    try:
      # https://www.kernel.org/doc/html/v6.18/filesystems/overlayfs.html
      #   The specified lower directories will be stacked
      #   beginning from the rightmost one and going left.
      # so we copy from right to left.
      for layer in reversed(lower):
        shutil.copytree(layer, merged, dirs_exist_ok = True)
      yield
    finally:
      subprocess.run(['umount', merged], check = False)
  else:
    raise RuntimeError("unsupported platform")

def remove_info_main_menu(prefix: Path):
  info_main_menu = prefix / 'share/info/dir'
  if info_main_menu.exists():
    info_main_menu.unlink()

def touch(path: Path):
  ensure(path.parent)
  path.touch(exist_ok = True)

def xmake_build(cwd: Path, jobs: int, targets: List[str] = []):
  subprocess.run(
    ['xmake', 'build', '-j', str(jobs), *targets],
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
