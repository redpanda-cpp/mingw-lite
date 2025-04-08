import argparse
import logging
from packaging.version import Version
import shutil
import subprocess

from module.debug import shell_here
from module.path import ProjectPaths
from module.profile import BranchProfile
from module.util import cflags_A, cflags_B, add_objects_to_static_lib, configure, ensure, fix_libtool_absolute_reference, make_custom, make_default, make_destdir_install, make_install

def _binutils(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.binutils / 'build-AAB'
  ensure(build_dir)
  configure('binutils', build_dir, [
    '--prefix=',
    f'--target={ver.target}',
    f'--build={config.build}',
    # static build
    '--disable-plugins',
    '--disable-shared',
    '--enable-static',
    '--disable-werror',
    # features
    '--disable-install-libbfd',
    '--disable-multilib',
    '--disable-nls',
    # libtool eats `-static`
    *cflags_A(ld_extra = ['--static']),
  ])
  make_default('binutils', build_dir, config.jobs)
  make_destdir_install('binutils', build_dir, paths.x_prefix)

def _headers(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.mingw / 'mingw-w64-headers' / 'build-AAB'
  ensure(build_dir)
  configure('headers', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    f'--with-default-msvcrt={ver.default_crt}',
    f'--with-default-win32-winnt=0x{ver.win32_winnt:04X}',
  ])
  make_default('headers', build_dir, config.jobs)
  make_destdir_install('headers', build_dir, paths.x_prefix / ver.target)

def _gcc(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.gcc)
  build_dir = paths.gcc / 'build-AAB'
  ensure(build_dir)

  config_flags = []

  if ver.exception == 'dwarf':
    config_flags.append('--disable-sjlj-exceptions')
    config_flags.append('--with-dwarf2')

  configure('gcc', build_dir, [
    f'--prefix={paths.x_prefix}',
    f'--libexecdir={paths.x_prefix / "lib"}',
    f'--with-gcc-major-version-only',
    f'--target={ver.target}',
    f'--build={config.build}',
    # static build
    '--disable-plugin',
    '--disable-shared',
    '--enable-static',
    '--without-pic',
    # features
    '--disable-bootstrap',
    '--enable-checking=release',
    '--enable-languages=c,c++',
    '--disable-libgomp',
    '--disable-libmpx',
    '--disable-lto',
    '--disable-multilib',
    '--disable-nls',
    f'--enable-threads={ver.thread}',
    # packages
    f'--with-gmp={paths.x_dep}',
    '--without-libcc1',
    f'--with-mpc={paths.x_dep}',
    f'--with-mpfr={paths.x_dep}',
    *config_flags,
    # libtool eats `-static`
    *cflags_A(ld_extra = ['--static']),
    *cflags_B('_FOR_TARGET',
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      ld_extra = ['--static'],
    ),
  ])

  make_custom('gcc (all-gcc)', build_dir, ['all-gcc'], config.jobs)
  make_custom('gcc (install-gcc)', build_dir, ['install-gcc'], jobs = 1)
  yield

  make_default('gcc', build_dir, config.jobs)
  make_install('gcc', build_dir)
  yield

def _crt(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.mingw / 'mingw-w64-crt' / 'build-AAB'
  ensure(build_dir)

  multilib_flags = [
    '--enable-lib64' if ver.arch == '64' else '--disable-lib64',
    '--enable-libarm64' if ver.arch == 'arm64' else '--disable-libarm64',
    '--enable-lib32' if ver.arch == '32' else '--disable-lib32',
    '--disable-libarm32',
  ]

  configure('crt', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    f'--with-default-msvcrt={ver.default_crt}',
    # use target definition since we use same source for both
    f'--with-default-win32-winnt=0x{ver.win32_winnt:04X}',
    *multilib_flags,
    *cflags_B(),
  ])
  make_default('crt', build_dir, config.jobs)
  make_destdir_install('crt', build_dir, paths.x_prefix / ver.target)

def _winpthreads(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.mingw / 'mingw-w64-libraries' / 'winpthreads' / 'build-AAB'
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
  make_destdir_install('winpthreads', build_dir, paths.x_prefix / ver.target)

def _mcfgthread(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.mcfgthread / 'build-AAB'
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
    '-j', str(config.jobs),
  ], cwd = build_dir)
  if ret.returncode != 0:
    message = 'Build failed: mcfgthread ninja returned %d' % ret.returncode
    logging.critical(message)
    raise Exception(message)
  shutil.copy(build_dir / 'libmcfgthread.a', paths.x_prefix / ver.target / 'lib' / 'libmcfgthread.a')

  include_dir = paths.x_prefix / ver.target / 'include' / 'mcfgthread'
  ensure(include_dir)
  header_files = [
    *paths.mcfgthread.glob('mcfgthread/*.h'),
    *paths.mcfgthread.glob('mcfgthread/*.hpp'),
    build_dir / 'version.h',
  ]
  for header_file in header_files:
    shutil.copy(header_file, include_dir / header_file.name)

def build_AAB_compiler(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _binutils(ver, paths, config)

  _headers(ver, paths, config)

  gcc = _gcc(ver, paths, config)
  gcc.__next__()

  _crt(ver, paths, config)

  _winpthreads(ver, paths, config)

  if ver.thread == 'mcf':
    _mcfgthread(ver, paths, config)

  gcc.__next__()

def _gmp(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.gmp)
  v_gcc = Version(ver.gcc)
  build_dir = paths.gmp / 'build-AAB'
  ensure(build_dir)

  c_extra = []

  # GCC 15 defaults to C23, in which `foo()` means `foo(void)` instead of `foo(...)`.
  if v_gcc.major >= 15 and v < Version('6.4.0'):
    c_extra.append('-std=gnu11')

  configure('gmp', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    '--disable-assembly',
    '--enable-static',
    '--disable-shared',
    *cflags_B(
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      c_extra = c_extra,
    ),
    # To determine build system compiler, the configure script will firstly try host
    # compiler (i.e. *-w64-mingw32-gcc) and check whether the output is executable
    # (and fallback to cc otherwise). However, in WSL or Linux with Wine configured,
    # the check passes and thus *-w64-mingw32-gcc is detected as build system compiler.
    # Here we force the build system compiler to be gcc.
    'CC_FOR_BUILD=gcc',
  ])
  make_default('gmp', build_dir, config.jobs)
  make_destdir_install('gmp', build_dir, paths.x_prefix / ver.target)

def _mpfr(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.mpfr / 'build-AAB'
  ensure(build_dir)
  configure('mpfr', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    '--enable-static',
    '--disable-shared',
    *cflags_B(
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
    ),
  ])
  make_default('mpfr', build_dir, config.jobs)
  make_destdir_install('mpfr', build_dir, paths.x_prefix / ver.target)

  fix_libtool_absolute_reference(paths.x_prefix / ver.target / 'lib' / 'libmpfr.la')

def _mpc(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.mpc / 'build-AAB'
  ensure(build_dir)
  configure('mpc', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    '--enable-static',
    '--disable-shared',
    *cflags_B(
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
    ),
  ])
  make_default('mpc', build_dir, config.jobs)
  make_destdir_install('mpc', build_dir, paths.x_prefix / ver.target)

  fix_libtool_absolute_reference(paths.x_prefix / ver.target / 'lib' / 'libmpc.la')

def _iconv(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.iconv)
  v_gcc = Version(ver.gcc)
  build_dir = paths.iconv / 'build-AAB'
  ensure(build_dir)

  c_extra = []

  # GCC 15 defaults to C23
  if v_gcc.major >= 15 and v < Version('1.18'):
    c_extra.append('-std=gnu11')

  configure('iconv', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    '--disable-nls',
    '--enable-static',
    '--disable-shared',
    *cflags_B(
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      c_extra = c_extra,
    ),
  ])
  make_default('iconv', build_dir, config.jobs)
  make_destdir_install('iconv', build_dir, paths.x_prefix / ver.target)

def _gettext(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.gettext / 'gettext-runtime' / 'build-AAB'
  ensure(build_dir)
  configure('gettext', build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    '--enable-static',
    '--disable-shared',
    *cflags_B(
      cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
    ),
  ])
  make_default('gettext', build_dir, config.jobs)
  make_destdir_install('gettext', build_dir, paths.x_prefix / ver.target)

  fix_libtool_absolute_reference(paths.x_prefix / ver.target / 'lib' / 'libintl.la')

def _python(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  XMAKE_ARCH_MAP = {
    '32': 'i386',
    '64': 'x86_64',
    'arm64': 'aarch64',
  }

  config_args = []
  if ver.min_os.major < 6:
    config_args.append('--emulated-win-cv=1')

  res = subprocess.run([
    'xmake', 'config', '--root',
    '-p', 'mingw',
    '-a', XMAKE_ARCH_MAP[ver.arch],
    f'--mingw={paths.x_prefix}',
    f'--ar={ver.target}-ar',
    f'--cc={ver.target}-gcc',
    f'--cxx={ver.target}-g++',
    f'--ld={ver.target}-g++',
    *config_args,
  ], cwd = paths.python)
  if res.returncode != 0:
    raise Exception('xmake config failed')
  res = subprocess.run([
    'xmake', 'build', '--root',
    '-j', str(config.jobs),
  ], cwd = paths.python)
  if res.returncode != 0:
    raise Exception('xmake build failed')
  res = subprocess.run([
    'xmake', 'install', '--root',
    '-o', paths.x_prefix / ver.target,
  ], cwd = paths.python)
  if res.returncode != 0:
    raise Exception('xmake install failed')

def _python_packages(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  x_prefix_mingw = paths.x_prefix / ver.target
  python_lib = x_prefix_mingw / 'Lib'
  python_lib_zip = x_prefix_mingw / 'lib' / 'python.zip'
  shutil.copytree(paths.x_prefix / 'share' / f'gcc-{config.branch}' / 'python', python_lib, dirs_exist_ok = True)
  subprocess.run([
    'python3', '-m', 'compileall',
    '-b',
    '-o', '2',
    '.',
  ], check = True, cwd = python_lib)
  if python_lib_zip.exists():
    python_lib_zip.unlink()
  subprocess.run([
    '7z', 'a', '-tzip',
    '-mx0',  # no compression, reduce final size
    python_lib_zip,
    '*', '-xr!__pycache__', '-xr!*.py',
  ], check = True, cwd = python_lib)

def build_AAB_library(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _gmp(ver, paths, config)

  _mpfr(ver, paths, config)

  _mpc(ver, paths, config)

  _iconv(ver, paths, config)

  if ver.gettext:
    _gettext(ver, paths, config)

  _python(ver, paths, config)
  _python_packages(ver, paths, config)
