import argparse
import logging
from shutil import copyfile
import subprocess

from module.debug import shell_here
from module.path import ProjectPaths
from module.profile import BranchVersions, ProfileInfo
from module.util import cflags_build, cflags_host, cflags_target, configure, ensure, make_default, make_install

def _binutils(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.binutils / 'build'
  ensure(build_dir)
  configure('binutils', build_dir, [
    f'--prefix={paths.prefix}',
    f'--host={info.target}',
    f'--target={info.target}',
    '--disable-nls',
    '--with-static-standard-libraries',
    *cflags_host(),
  ])
  make_default('binutils', build_dir, jobs)
  make_install('binutils', build_dir)

def _gmp(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.gmp / 'build'
  ensure(build_dir)
  configure('gmp', build_dir, [
    f'--prefix={paths.dep}',
    f'--host={info.target}',
    '--disable-assembly',
    '--enable-static',
    '--disable-shared',
    *cflags_host(),
  ])
  make_default('gmp', build_dir, jobs)
  make_install('gmp', build_dir)

def _mpfr(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mpfr / 'build'
  ensure(build_dir)
  configure('mpfr', build_dir, [
    f'--prefix={paths.dep}',
    f'--host={info.target}',
    f'--with-gmp-include={paths.dep}/include',
    f'--with-gmp-lib={paths.dep}/lib',
    '--enable-static',
    '--disable-shared',
    *cflags_host(),
  ])
  make_default('mpfr', build_dir, jobs)
  make_install('mpfr', build_dir)

def _mpc(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mpc / 'build'
  ensure(build_dir)
  configure('mpc', build_dir, [
    f'--prefix={paths.dep}',
    f'--host={info.target}',
    f'--with-gmp-include={paths.dep}/include',
    f'--with-gmp-lib={paths.dep}/lib',
    f'--with-mpfr-include={paths.dep}/include',
    f'--with-mpfr-lib={paths.dep}/lib',
    '--enable-static',
    '--disable-shared',
    *cflags_host(),
  ])
  make_default('mpc', build_dir, jobs)
  make_install('mpc', build_dir)

def _headers(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mingw / 'mingw-w64-headers' / 'build'
  ensure(build_dir)
  configure('mingw-w64-headers', build_dir, [
    f'--prefix={paths.prefix}',
    f'--host={info.target}',
    f'--with-default-msvcrt={info.crt}',
    f'--with-default-win32-winnt=0x{info.target_winnt:04X}',
  ])
  make_default('headers', build_dir, jobs)
  make_install('headers', build_dir)

def _crt(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mingw / 'mingw-w64-crt' / 'build'
  multilib_flags = [
    '--enable-lib64' if info.arch == '64' else '--disable-lib64',
    '--enable-libarm64' if info.arch == 'arm64' else '--disable-libarm64',
    '--enable-lib32' if info.arch == '32' else '--disable-lib32',
    '--disable-libarm32',
  ]
  ensure(build_dir)
  configure('mingw-w64-crt', build_dir, [
    f'--prefix={paths.prefix}/{info.target}',
    f'--with-sysroot={paths.prefix}/{info.target}',
    f'--host={info.target}',
    f'--with-default-msvcrt={info.crt}',
    f'--with-default-win32-winnt=0x{info.target_winnt:04X}',
    '--disable-dependency-tracking',
    *multilib_flags,
    *cflags_target(),
  ])
  make_default('crt', build_dir, jobs)
  make_install('crt', build_dir)

def _winpthreads(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mingw / 'mingw-w64-libraries' / 'winpthreads' / 'build'
  ensure(build_dir)
  configure('winpthreads', build_dir, [
    f'--prefix={paths.prefix}/{info.target}',
    f'--with-sysroot={paths.prefix}/{info.target}',
    f'--host={info.target}',
    '--enable-static',
    '--disable-shared',
    *cflags_target(),
  ])
  make_default('winpthreads', build_dir, jobs)
  make_install('winpthreads', build_dir)

def _mcfgthread(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mcfgthread / 'build'
  ensure(build_dir)
  ret = subprocess.run([
    'meson',
    'setup',
    '--cross-file', f'meson.cross.{info.target}',
    build_dir,
  ], cwd = paths.mcfgthread)
  if ret.returncode != 0:
    message = 'Build failed: mcfgthread meson setup returned %d' % ret.returncode
    logging.critical(message)
    raise Exception(message)
  ret = subprocess.run([
    'env',
    *cflags_host(),
    'meson',
    'compile',
    'mcfgthread:static_library',
  ], cwd = build_dir)
  if ret.returncode != 0:
    message = 'Build failed: mcfgthread ninja returned %d' % ret.returncode
    logging.critical(message)
    raise Exception(message)
  copyfile(build_dir / 'libmcfgthread.a', paths.prefix / info.target / 'lib' / 'libmcfgthread.a')

  include_dir = paths.prefix / info.target / 'include' / 'mcfgthread'
  ensure(include_dir)
  header_files = [
    *paths.mcfgthread.glob('mcfgthread/*.h'),
    *paths.mcfgthread.glob('mcfgthread/*.hpp'),
    build_dir / 'version.h',
  ]
  for header_file in header_files:
    copyfile(header_file, include_dir / header_file.name)

def _iconv(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.iconv / 'build'
  ensure(build_dir)
  configure('iconv', build_dir, [
    f'--prefix={paths.dep}',
    f'--host={info.target}',
    '--disable-nls',
    '--enable-static',
    '--disable-shared',
    *cflags_host(),
  ])
  make_default('iconv', build_dir, jobs)
  make_install('iconv', build_dir)

def _gettext(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.gettext / 'gettext-runtime' / 'build'
  ensure(build_dir)
  configure('gettext', build_dir, [
    f'--prefix={paths.dep}',
    f'--host={info.target}',
    '--enable-static',
    '--disable-shared',
    *cflags_host(),
  ])
  make_default('gettext', build_dir, jobs)
  make_install('gettext', build_dir)

def _gcc(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.gcc / 'build'
  exception_flags = [
    '--with-dwarf2',
    '--disable-sjlj-exceptions',
  ] if info.exception == 'dwarf' else []
  ensure(build_dir)
  configure('gcc', build_dir, [
    f'--prefix={paths.prefix}',
    f'--with-sysroot={paths.prefix}/{info.target}',
    '--with-native-system-header-dir=/include',
    f'--target={info.target}',
    f'--host={info.target}',
    '--enable-static',
    '--disable-shared',
    '--with-pic',
    f'--with-gmp={paths.dep}',
    f'--with-mpfr={paths.dep}',
    f'--with-mpc={paths.dep}',
    f'--with-libiconv-prefix={paths.dep}',
    f'--with-libintl-prefix={paths.dep}',
    '--enable-languages=c,c++',
    '--enable-libgomp',
    f'--enable-threads={info.thread}',
    *exception_flags,
    '--enable-version-specific-runtime-libs',
    '--disable-dependency-tracking',
    '--disable-multilib',
    '--enable-nls',
    '--disable-win32-registry',
    '--enable-mingw-wildcard',
    *cflags_build('_FOR_BUILD'),
    *cflags_host(),
    *cflags_target('_FOR_TARGET'),
  ])
  make_default('gcc', build_dir, jobs)
  make_install('gcc', build_dir)

def _gdb(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.gdb / 'build'
  ensure(build_dir)
  configure('gdb', build_dir, [
    f'--prefix={paths.prefix}',
    f'--host={info.target}',
    f'--with-gmp={paths.dep}',
    f'--with-mpfr={paths.dep}',
    f'--with-mpc={paths.dep}',
    '--disable-tui',
    *cflags_host(common_extra = ['-DPDC_WIDE']),
  ])
  make_default('gdb', build_dir, jobs)
  make_install('gdb', build_dir)

def _gmake(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.make / 'build'
  ensure(build_dir)
  configure('make', build_dir, [
    f'--prefix={paths.prefix}',
    f'--host={info.target}',
    '--disable-nls',
    *cflags_host(),
  ])
  make_default('make', build_dir, jobs)
  copyfile(build_dir / 'make.exe', paths.prefix / 'bin' / 'mingw32-make.exe')

def _licenses(ver: BranchVersions, paths: ProjectPaths, info: ProfileInfo):
  license_dir = paths.prefix / 'license'
  ensure(license_dir)

  copyfile(paths.binutils / 'COPYING', license_dir / 'binutils.txt')
  copyfile(paths.binutils / 'COPYING.LIB', license_dir / 'binutils-lib.txt')
  copyfile(paths.binutils / 'COPYING3', license_dir / 'binutils3.txt')
  copyfile(paths.binutils / 'COPYING3.LIB', license_dir / 'binutils3-lib.txt')

  copyfile(paths.gcc / 'COPYING', license_dir / 'gcc.txt')
  copyfile(paths.gcc / 'COPYING.LIB', license_dir / 'gcc-lib.txt')
  copyfile(paths.gcc / 'COPYING.RUNTIME', license_dir / 'gcc-runtime.txt')
  copyfile(paths.gcc / 'COPYING3', license_dir / 'gcc3.txt')
  copyfile(paths.gcc / 'COPYING3.LIB', license_dir / 'gcc3-lib.txt')

  copyfile(paths.gdb / 'COPYING', license_dir / 'gdb.txt')
  copyfile(paths.gdb / 'COPYING.LIB', license_dir / 'gdb-lib.txt')
  copyfile(paths.gdb / 'COPYING3', license_dir / 'gdb3.txt')
  copyfile(paths.gdb / 'COPYING3.LIB', license_dir / 'gdb3-lib.txt')

  copyfile(paths.gettext / 'gettext-runtime' / 'intl' / 'COPYING.LIB', license_dir / 'gettext-lib.txt')

  copyfile(paths.gmp / 'COPYING', license_dir / 'gmp.txt')
  copyfile(paths.gmp / 'COPYINGv2', license_dir / 'gmpv2.txt')
  copyfile(paths.gmp / 'COPYING.LESSERv3', license_dir / 'gmp-lesserv3.txt')

  copyfile(paths.iconv / 'COPYING', license_dir / 'iconv.txt')
  copyfile(paths.iconv / 'COPYING.LIB', license_dir / 'iconv-lib.txt')

  copyfile(paths.make / 'COPYING', license_dir / 'make.txt')

  if info.thread == 'mcf':
    copyfile(paths.mcfgthread / 'LICENSE.TXT', license_dir / 'mcfgthread.txt')

  copyfile(paths.mingw / 'COPYING', license_dir / 'mingw-w64.txt')
  copyfile(paths.mingw / 'mingw-w64-libraries' / 'winpthreads' / 'COPYING', license_dir / 'winpthreads.txt')

  copyfile(paths.mpc / 'COPYING.LESSER', license_dir / 'mpc-lesser.txt')

  copyfile(paths.mpfr / 'COPYING', license_dir / 'mpfr.txt')
  copyfile(paths.mpfr / 'COPYING.LESSER', license_dir / 'mpfr-lesser.txt')

def build_mingw_toolchain(ver: BranchVersions, paths: ProjectPaths, info: ProfileInfo, config: argparse.Namespace):
  _binutils(ver.binutils, paths, info, config.jobs)

  _gmp(ver.gmp, paths, info, config.jobs)

  _mpfr(ver.mpfr, paths, info, config.jobs)

  _mpc(ver.mpc, paths, info, config.jobs)

  _headers(ver.mingw, paths, info, config.jobs)

  _crt(ver.mingw, paths, info, config.jobs)

  _winpthreads(ver.mingw, paths, info, config.jobs)

  if info.thread == 'mcf':
    _mcfgthread(ver.mcfgthread, paths, info, config.jobs)

  _iconv(ver.iconv, paths, info, config.jobs)

  _gettext(ver.gettext, paths, info, config.jobs)

  _gcc(ver.gcc, paths, info, config.jobs)

  _gdb(ver.gdb, paths, info, config.jobs)

  _gmake(ver.make, paths, info, config.jobs)

  _licenses(ver, paths, info)
