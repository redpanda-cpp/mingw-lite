import argparse
import logging
from shutil import copyfile
import subprocess

from module.debug import shell_here
from module.path import ProjectPaths
from module.profile import BranchVersions, ProfileInfo
from module.util import cflags_build, cflags_host, configure, ensure, make_custom, make_default, make_install

def _binutils(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.binutils / 'x-build'
  ensure(build_dir)
  configure('binutils', build_dir, [
    f'--prefix={paths.x_prefix}',
    f'--target={info.target}',
    '--disable-nls',
    '--with-static-standard-libraries',
    '--disable-multilib',
    *cflags_build(),
  ])
  make_default('binutils', build_dir, jobs)
  make_install('binutils', build_dir)

def _headers(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mingw / 'mingw-w64-headers' / 'x-build'
  ensure(build_dir)
  configure('headers', build_dir, [
    f'--prefix={paths.x_prefix / info.target}',
    f'--host={info.target}',
    f'--with-default-msvcrt={info.crt}',
    f'--with-default-win32-winnt=0x{max(info.host_winnt, 0x0400):04X}',
  ])
  make_default('headers', build_dir, jobs)
  make_install('headers', build_dir)

def _gmp(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.gmp / 'x-build'
  ensure(build_dir)
  configure('gmp', build_dir, [
    f'--prefix={paths.x_dep}',
    '--disable-assembly',
    '--enable-static',
    '--disable-shared',
    *cflags_build(),
  ])
  make_default('gmp', build_dir, jobs)
  make_install('gmp', build_dir)

def _mpfr(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mpfr / 'x-build'
  ensure(build_dir)
  configure('mpfr', build_dir, [
    f'--prefix={paths.x_dep}',
    f'--with-gmp={paths.x_dep}',
    '--enable-static',
    '--disable-shared',
    *cflags_build(),
  ])
  make_default('mpfr', build_dir, jobs)
  make_install('mpfr', build_dir)

def _mpc(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mpc / 'x-build'
  ensure(build_dir)
  configure('mpc', build_dir, [
    f'--prefix={paths.x_dep}',
    f'--with-gmp={paths.x_dep}',
    f'--with-mpfr={paths.x_dep}',
    '--enable-static',
    '--disable-shared',
    *cflags_build(),
  ])
  make_default('mpc', build_dir, jobs)
  make_install('mpc', build_dir)

def _gcc(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.gcc / 'x-build'
  exception_flags = [
    '--with-dwarf2',
    '--disable-sjlj-exceptions',
  ] if info.exception == 'dwarf' else []
  ensure(build_dir)
  configure('gcc', build_dir, [
    f'--prefix={paths.x_prefix}',
    f'--libexecdir={paths.x_prefix / 'lib'}',
    f'--with-gcc-major-version-only',
    f'--target={info.target}',
    '--enable-static',
    '--disable-shared',
    '--with-pic',
    f'--with-gmp={paths.x_dep}',
    f'--with-mpfr={paths.x_dep}',
    f'--with-mpc={paths.x_dep}',
    '--enable-languages=c,c++',
    '--enable-libgomp',
    f'--enable-threads={info.thread}',
    *exception_flags,
    '--enable-version-specific-runtime-libs',
    '--disable-dependency-tracking',
    '--disable-nls',
    '--disable-multilib',
    *cflags_build('_FOR_BUILD'),  # In the cross phase,
    *cflags_build(),              # GCC’s HOST is our BUILD (Linux x86_64)
    *cflags_host('_FOR_TARGET'),  # GCC’s TARGET is our HOST (Windows on which GCC runs)
  ])
  make_custom('gcc (all-gcc)', build_dir, ['all-gcc'], jobs)
  make_custom('gcc (install-gcc)', build_dir, ['install-gcc'], jobs = 1)
  yield

  make_default('gcc', build_dir, jobs)
  make_install('gcc', build_dir)
  yield

def _crt(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mingw / 'mingw-w64-crt' / 'x-build'
  multilib_flags = [
    '--enable-lib64' if info.arch == '64' else '--disable-lib64',
    '--enable-libarm64' if info.arch == 'arm64' else '--disable-libarm64',
    '--enable-lib32' if info.arch == '32' else '--disable-lib32',
    '--disable-libarm32',
  ]
  ensure(build_dir)
  configure('crt', build_dir, [
    f'--prefix={paths.x_prefix / info.target}',
    f'--with-sysroot={paths.x_prefix / info.target}',
    f'--host={info.target}',
    f'--with-default-msvcrt={info.crt}',
    f'--with-default-win32-winnt=0x{max(info.host_winnt, 0x0400):04X}',
    '--disable-dependency-tracking',
    *multilib_flags,
    *cflags_host(),
  ])
  make_default('crt', build_dir, jobs)
  make_install('crt', build_dir)

def _winpthreads(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mingw / 'mingw-w64-libraries' / 'winpthreads' / 'x-build'
  ensure(build_dir)
  configure('winpthreads', build_dir, [
    f'--prefix={paths.x_prefix / info.target}',
    f'--with-sysroot={paths.x_prefix / info.target}',
    f'--host={info.target}',
    '--enable-static',
    '--disable-shared',
    *cflags_host(),
  ])
  make_default('winpthreads', build_dir, jobs)
  make_install('winpthreads', build_dir)

def _mcfgthread(ver: str, paths: ProjectPaths, info: ProfileInfo, jobs: int):
  build_dir = paths.mcfgthread / 'x-build'
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
  copyfile(build_dir / 'libmcfgthread.a', paths.x_prefix / info.target / 'lib' / 'libmcfgthread.a')

  include_dir = paths.x_prefix / info.target / 'include' / 'mcfgthread'
  ensure(include_dir)
  header_files = [
    *paths.mcfgthread.glob('mcfgthread/*.h'),
    *paths.mcfgthread.glob('mcfgthread/*.hpp'),
    build_dir / 'version.h',
  ]
  for header_file in header_files:
    copyfile(header_file, include_dir / header_file.name)

def build_cross_compiler(ver: BranchVersions, paths: ProjectPaths, info: ProfileInfo, config: argparse.Namespace):
  _binutils(ver.binutils, paths, info, config.jobs)

  _headers(ver.mingw, paths, info, config.jobs)

  _gmp(ver.gmp, paths, info, config.jobs)

  _mpfr(ver.mpfr, paths, info, config.jobs)

  _mpc(ver.mpc, paths, info, config.jobs)

  gcc = _gcc(ver.gcc, paths, info, config.jobs)
  gcc.__next__()

  _crt(ver.mingw, paths, info, config.jobs)

  _winpthreads(ver.mingw, paths, info, config.jobs)

  if info.thread == 'mcf':
    _mcfgthread(ver.mcfgthread, paths, info, config.jobs)

  gcc.__next__()
