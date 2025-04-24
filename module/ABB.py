import argparse
import logging
import os
from packaging.version import Version
from pathlib import Path
import shutil
import subprocess
from typing import Optional

from module.debug import shell_here
from module.path import ProjectPaths
from module.profile import BranchProfile
from module.util import XMAKE_ARCH_MAP, add_objects_to_static_lib, cflags_B, configure, ensure, make_custom, make_default, make_destdir_install, make_install, xmake_build, xmake_config

def _binutils(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.binutils / 'build-ABB'
  ensure(build_dir)
  configure('binutils', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--target={ver.target}',
    f'--build={config.build}',
    # static build
    '--with-static-standard-libraries',
    # features
    '--disable-install-libbfd',
    '--disable-multilib',
    '--disable-nls',
    # libtool eats `-static`
    *cflags_B(
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      ld_extra = ['--static'],
    ),
  ])
  make_default('binutils', build_dir, config.jobs)
  make_custom('binutils (install)', build_dir, [
    f'DESTDIR={paths.mingw_prefix}',
    # use native layout
    'tooldir=',
    'install',
  ], jobs = 1)

def _headers(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.mingw_target / 'mingw-w64-headers' / 'build-ABB'
  ensure(build_dir)
  configure('mingw-w64-headers', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    f'--with-default-msvcrt={ver.default_crt}',
    f'--with-default-win32-winnt=0x{ver.win32_winnt:04X}',
  ])
  make_default('headers', build_dir, config.jobs)
  make_destdir_install('headers', build_dir, paths.mingw_prefix)

def _crt(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.mingw_target / 'mingw-w64-crt' / 'build-ABB'
  ensure(build_dir)

  multilib_flags = [
    '--enable-lib64' if ver.arch == '64' else '--disable-lib64',
    '--enable-libarm64' if ver.arch == 'arm64' else '--disable-libarm64',
    '--enable-lib32' if ver.arch == '32' else '--disable-lib32',
    '--disable-libarm32',
  ]

  configure('mingw-w64-crt', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    f'--with-default-msvcrt={ver.default_crt}',
    f'--with-default-win32-winnt=0x{max(ver.win32_winnt, 0x0400):04X}',
    *multilib_flags,
    *cflags_B(),
  ])
  make_default('crt', build_dir, config.jobs)
  make_destdir_install('crt', build_dir, paths.mingw_prefix)

def _winpthreads(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.mingw_target / 'mingw-w64-libraries' / 'winpthreads' / 'build-ABB'
  ensure(build_dir)
  configure('winpthreads', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    '--enable-static',
    '--disable-shared',
    *cflags_B(
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
    ),
  ])
  make_default('winpthreads', build_dir, config.jobs)
  make_destdir_install('winpthreads', build_dir, paths.mingw_prefix)

def _mcfgthread(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.mcfgthread / 'build-ABB'
  ensure(build_dir)

  ret = subprocess.run([
    'meson',
    'setup',
    '--cross-file', f'meson.cross.{ver.target}',
    build_dir,
  ], cwd = paths.mcfgthread)
  if ret.returncode != 0:
    message = 'Build failed: mcfgthread meson setup returned %d' % ret.returncode
    logging.critical(message)
    raise Exception(message)
  ret = subprocess.run([
    'env',
    *cflags_B(
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
    ),
    'meson',
    'compile',
    'mcfgthread:static_library',
  ], cwd = build_dir)
  if ret.returncode != 0:
    message = 'Build failed: mcfgthread ninja returned %d' % ret.returncode
    logging.critical(message)
    raise Exception(message)
  shutil.copy(build_dir / 'libmcfgthread.a', paths.mingw_prefix / 'lib' / 'libmcfgthread.a')

  include_dir = paths.mingw_prefix / 'include' / 'mcfgthread'
  ensure(include_dir)
  header_files = [
    *paths.mcfgthread.glob('mcfgthread/*.h'),
    *paths.mcfgthread.glob('mcfgthread/*.hpp'),
    build_dir / 'version.h',
  ]
  for header_file in header_files:
    shutil.copy(header_file, include_dir / header_file.name)

def _gcc(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.gcc)
  build_dir = paths.gcc / 'build-ABB'
  ensure(build_dir)

  config_flags = []

  if ver.exception == 'dwarf':
    config_flags.append('--disable-sjlj-exceptions')
    config_flags.append('--with-dwarf2')
  if ver.min_os.major < 6:
    config_flags.append('--disable-win32-utf8-manifest')

  configure('gcc', build_dir, [
    '--prefix=',
    '--libexecdir=/lib',
    f'--with-gcc-major-version-only',
    f'--target={ver.target}',
    f'--host={ver.target}',
    f'--build={config.build}',
    # prefer static
    '--disable-plugin',
    '--disable-shared',
    '--enable-static',
    '--without-pic',
    # features
    '--disable-bootstrap',
    '--enable-checking=release',
    '--enable-languages=c,c++',
    '--disable-libmpx',
    '--disable-multilib',
    '--enable-nls',
    f'--enable-threads={ver.thread}',
    '--disable-win32-registry',
    # packages
    '--without-libcc1',
    '--with-libiconv',
    *config_flags,
    *cflags_B(
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      ld_extra = ['--static'],
    ),
    *cflags_B('_FOR_TARGET',
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      # C++ standard library:
      #   ostream &ostream::operator<<(ostream &(*func)(ostream &))
      #   {
      #     return func(*this);
      #   }
      # is compiled into single instruction:
      #   jmp *rdx
      # hence there's no complete frame info.
      #
      # when debugging, gdb fails to resolve frame info,
      # and will stop here if we want to "step over" it.
      #
      # here we add minimal debug info, so gdb will not be fooled.
      common_extra = ['-g1'],
      ld_extra = ['--static'],
    ),
  ])
  make_default('gcc', build_dir, config.jobs)
  make_destdir_install('gcc', build_dir, paths.mingw_prefix)

  # add `print.o` to `libstdc++.a`, allowing `<print>` without `-lstdc++exp`
  # it's okay since we only keep ABI stable in a major version
  if 14 <= v.major < 16:
    res = subprocess.run([
      f'{ver.target}-ar', 'r',
      paths.mingw_prefix / 'lib' / 'libstdc++.a',
      build_dir / ver.target / 'libstdc++-v3' / 'src' / 'c++23' / 'print.o',
    ])
    if res.returncode != 0:
      message = f'Build fail: libstdc++ ar returned {res.returncode}'
      logging.critical(message)
      raise Exception(message)

def _gdb(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.gdb)
  v_gcc = Version(ver.gcc)
  build_dir = paths.gdb / 'build-ABB'
  ensure(build_dir)

  python_flags = []
  c_extra = []

  python_flags.append(f'--with-python={paths.x_prefix}/{ver.target}/python-config.sh')

  # GCC 15 defaults to C23, in which `foo()` means `foo(void)` instead of `foo(...)`.
  if v_gcc.major >= 15 and v < Version('16.3'):
    c_extra.append('-std=gnu11')

  cflags = cflags_B(
    cpp_extra = [
      f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}',
      '-DPDC_WIDE',
      # workaround: bfd and gnulib disagree about i686 time_t
      '-D__MINGW_USE_VC2005_COMPAT',
    ],
    c_extra = c_extra,
  )

  configure('gdb', build_dir, [
    '--prefix=',
    f'--target={ver.target}',
    f'--host={ver.target}',
    f'--build={config.build}',
    # features
    '--disable-tui',
    # packages
    f'--with-system-gdbinit=/share/gdb/gdbinit',
    *python_flags,
    *cflags,
  ])

  # workaround: top level configure does not pass CPPFLAGS to gdbsupport.
  # thus it defaults to _WIN32_WINNT=0x0501, breaking win32 and mcf thread model.
  for item in cflags:
    if item.startswith('CPPFLAGS='):
      os.environ['CPPFLAGS'] = item[len('CPPFLAGS='):]
  make_custom('gdb (configure-host)', build_dir, ['configure-host'], config.jobs)
  del os.environ['CPPFLAGS']

  make_default('gdb', build_dir, config.jobs)
  make_destdir_install('gdb', build_dir, paths.mingw_prefix)

  gdbinit = paths.mingw_prefix / 'share' / 'gdb' / 'gdbinit'
  with open(gdbinit, 'w') as f:
    pass

  shutil.copy(paths.x_prefix / ver.target / 'lib' / 'python.zip', paths.mingw_prefix / 'lib' / 'python.zip')
  with open(paths.mingw_prefix / 'bin' / 'gdb._pth', 'w') as f:
    f.write('../lib/python.zip\n')
  with open(gdbinit, 'a') as f:
    f.write('python\n')
    f.write('from libstdcxx.v6.printers import register_libstdcxx_printers\n')
    f.write('register_libstdcxx_printers(None)\n')
    f.write('end\n')

def _gmake(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.make)
  v_gcc = Version(ver.gcc)
  build_dir = paths.make / 'build-ABB'
  ensure(build_dir)

  c_extra = []

  # GCC 15 defaults to C23, in which `foo()` means `foo(void)` instead of `foo(...)`.
  if v_gcc.major >= 15 and v < Version('4.5'):
    c_extra.append('-std=gnu11')

  configure('make', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    '--disable-nls',
    *cflags_B(
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      c_extra = c_extra,
    ),
  ])
  make_default('make', build_dir, config.jobs)
  shutil.copy(build_dir / 'make.exe', paths.mingw_prefix / 'bin' / 'mingw32-make.exe')

def _xmake(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.xmake / 'core'
  xmake_config('xmake', build_dir, [
    '--plat=mingw',
    f'--arch={XMAKE_ARCH_MAP[ver.arch]}',
    '--toolchain=cross',
    f'--sdk={paths.x_prefix}',
    f'--cross={ver.target}-',
    '--embed=y',
  ])
  xmake_build('xmake', build_dir, config.jobs)
  shutil.copy(build_dir / 'build' / 'xmake.exe', paths.mingw_prefix / 'bin' / 'xmake.exe')

def _licenses(ver: BranchProfile, paths: ProjectPaths):
  license_dir = paths.mingw_prefix / 'share' / 'licenses'
  ensure(license_dir)

  ensure(license_dir / 'binutils')
  for file in ['README', 'COPYING', 'COPYING3', 'COPYING.LIB', 'COPYING3.LIB']:
    shutil.copy(paths.binutils / 'COPYING3', license_dir / 'binutils' / 'COPYING3')

  ensure(license_dir / 'gcc')
  for file in ['README', 'COPYING', 'COPYING3', 'COPYING.RUNTIME', 'COPYING.LIB', 'COPYING3.LIB']:
    shutil.copy(paths.gcc / file, license_dir / 'gcc' / file)

  ensure(license_dir / 'gdb')
  for file in ['README', 'COPYING', 'COPYING3', 'COPYING.LIB', 'COPYING3.LIB']:
    shutil.copy(paths.gdb / file, license_dir / 'gdb' / file)

  if ver.gettext:
    ensure(license_dir / 'gettext-runtime-intl')
    shutil.copy(paths.gettext / 'gettext-runtime' / 'intl' / 'COPYING.LIB', license_dir / 'gettext-runtime-intl' / 'COPYING.LIB')

  ensure(license_dir / 'gmp')
  for file in ['README', 'COPYINGv2', 'COPYINGv3', 'COPYING.LESSERv3']:
    shutil.copy(paths.gmp / file, license_dir / 'gmp' / file)

  ensure(license_dir / 'iconv')
  shutil.copy(paths.iconv / 'COPYING.LIB', license_dir / 'iconv' / 'COPYING.LIB')

  ensure(license_dir / 'make')
  shutil.copy(paths.make / 'COPYING', license_dir / 'make' / 'COPYING')

  if ver.thread == 'mcf':
    ensure(license_dir / 'mcfgthread')
    shutil.copy(paths.mcfgthread / 'LICENSE.TXT', license_dir / 'mcfgthread' / 'LICENSE.TXT')
    for file in ['gcc-exception-3.1.txt', 'gpl-3.0.txt', 'lgpl-3.0.txt']:
      shutil.copy(paths.mcfgthread / 'licenses' / file, license_dir / 'mcfgthread' / file)

  ensure(license_dir / 'mingw-w64')
  shutil.copy(paths.mingw_target / 'COPYING', license_dir / 'mingw-w64' / 'COPYING')

  ensure(license_dir / 'mingw-w64-libraries-winpthreads')
  shutil.copy(paths.mingw_target / 'mingw-w64-libraries' / 'winpthreads' / 'COPYING', license_dir / 'mingw-w64-libraries-winpthreads' / 'COPYING')

  ensure(license_dir / 'mpc')
  shutil.copy(paths.mpc / 'COPYING.LESSER', license_dir / 'mpc' / 'COPYING.LESSER')

  ensure(license_dir / 'mpfr')
  shutil.copy(paths.mpfr / 'COPYING.LESSER', license_dir / 'mpfr' / 'COPYING.LESSER')

  ensure(license_dir / 'python')
  shutil.copy(paths.python / 'LICENSE', license_dir / 'python' / 'LICENSE')

  ensure(license_dir / 'xmake')
  shutil.copy(paths.xmake / 'LICENSE.md', license_dir / 'xmake' / 'LICENSE.md')

  ensure(license_dir / 'zlib')
  shutil.copy(paths.python_z / 'LICENSE', license_dir / 'zlib' / 'LICENSE')

def build_ABB_toolchain(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _binutils(ver, paths, config)

  _headers(ver, paths, config)

  _crt(ver, paths, config)

  _winpthreads(ver, paths, config)

  if ver.thread == 'mcf':
    _mcfgthread(ver, paths, config)

  _gcc(ver, paths, config)

  _gdb(ver, paths, config)

  _gmake(ver, paths, config)

  _xmake(ver, paths, config)

  _licenses(ver, paths)
