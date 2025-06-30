import argparse
import logging
from packaging.version import Version
import shutil
import subprocess

from module.debug import shell_here
from module.path import ProjectPaths
from module.profile import BranchProfile
from module.util import XMAKE_ARCH_MAP, add_objects_to_static_lib, cflags_A, cflags_B, configure, ensure, make_custom, make_default, make_destdir_install, overlayfs_ro, xmake_build, xmake_config, xmake_install

def _binutils(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.src_dir.binutils / 'build-AAB'
  ensure(build_dir)
  configure('binutils', build_dir, [
    f'--prefix=/usr/local',
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
  make_destdir_install('binutils', build_dir, paths.layer_AAB.binutils)

def _headers(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.src_dir.mingw_host / 'mingw-w64-headers' / 'build-AAB'
  ensure(build_dir)
  configure('headers', build_dir, [
    f'--prefix=/usr/local/{ver.target}',
    f'--host={ver.target}',
    f'--build={config.build}',
    f'--with-default-msvcrt={ver.default_crt}',
    f'--with-default-win32-winnt=0x{ver.win32_winnt:04X}',
  ])
  make_default('headers', build_dir, config.jobs)
  make_destdir_install('headers', build_dir, paths.layer_AAB.headers)
  yield

  include_dir = paths.layer_AAB.headers / 'usr/local' / ver.target / 'include'
  for dummy_header in ['pthread_signal.h', 'pthread_time.h', 'pthread_unistd.h']:
    (include_dir / dummy_header).unlink()
  yield

def _gcc(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.gmp / 'usr/local',
    paths.layer_AAA.mpc / 'usr/local',
    paths.layer_AAA.mpfr / 'usr/local',

    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
  ]):
    build_dir = paths.src_dir.gcc / 'build-AAB'
    ensure(build_dir)

    config_flags = []

    if ver.exception == 'dwarf':
      config_flags.append('--disable-sjlj-exceptions')
      config_flags.append('--with-dwarf2')
    if ver.fpmath:
      config_flags.append(f'--with-fpmath={ver.fpmath}')

    configure('gcc', build_dir, [
      f'--prefix=/usr/local',
      f'--libexecdir=/usr/local/lib',
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
      f'--with-arch={ver.march}',
      '--without-libcc1',
      *config_flags,
      # libtool eats `-static`
      *cflags_A(ld_extra = ['--static']),
      *cflags_B('_FOR_TARGET',
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        ld_extra = ['--static'],
      ),
    ])

    make_custom('gcc (all-gcc)', build_dir, ['all-gcc'], config.jobs)
    make_custom('gcc (install-gcc)', build_dir, [
      f'DESTDIR={paths.layer_AAB.gcc}',
      'install-gcc',
    ], jobs = 1)
  yield

  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.gmp / 'usr/local',
    paths.layer_AAA.mpc / 'usr/local',
    paths.layer_AAA.mpfr / 'usr/local',

    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',

    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',
  ]):
    # target missing atomic instructions, and gcc will yield function calls.
    # here we add libatomic to libgcc, so we don't need to handle build flags later.
    if ver.march in ['i386', 'i486']:
      make_custom('gcc (all-target-libgcc)', build_dir, ['all-target-libgcc'], config.jobs)

      libgcc_a = build_dir / ver.target / 'libgcc' / 'libgcc.a'
      xgcc_libgcc_a = build_dir / 'gcc' / 'libgcc.a'

      # bootstrapping i386 libatomic is tricky.
      # the configure script checks whether GCC can compile program.
      # however, CRT startup code depends on libatomic, which is not built yet.
      # here we provide fake symbols to pass the check.
      if ver.march == 'i386':
        libatomic_fake_object = build_dir / 'cas_4_.o'
        subprocess.run([
          f'{ver.target}-gcc', '-x', 'c', '-c', '-', '-o', libatomic_fake_object,
        ], check = True, input = b''.join([
          b'int __atomic_compare_exchange_1;',
          b'int __atomic_compare_exchange_2;',
          b'int __atomic_compare_exchange_4;',
        ]))

        add_objects_to_static_lib(f'{ver.target}-ar', xgcc_libgcc_a, [libatomic_fake_object])

      make_custom('gcc (all-target-libatomic)', build_dir, ['all-target-libatomic'], config.jobs)

      atomic_objects = (build_dir / ver.target / 'libatomic').glob('*.o')
      add_objects_to_static_lib(f'{ver.target}-ar', libgcc_a, atomic_objects)
      shutil.copy(libgcc_a, xgcc_libgcc_a)

    make_default('gcc', build_dir, config.jobs)
    make_destdir_install('gcc', build_dir, paths.layer_AAB.gcc)
  yield

def _crt(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
  ]):
    build_dir = paths.src_dir.mingw_host / 'mingw-w64-crt' / 'build-AAB'
    ensure(build_dir)

    multilib_flags = [
      '--enable-lib64' if ver.arch == '64' else '--disable-lib64',
      '--enable-libarm64' if ver.arch == 'arm64' else '--disable-libarm64',
      '--enable-lib32' if ver.arch == '32' else '--disable-lib32',
      '--disable-libarm32',
    ]

    configure('crt', build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      f'--with-default-msvcrt={ver.default_crt}',
      # use target definition since we use same source for both
      f'--with-default-win32-winnt=0x{ver.win32_winnt:04X}',
      *multilib_flags,
      *cflags_B(),
    ])
    make_default('crt', build_dir, config.jobs)
    make_destdir_install('crt', build_dir, paths.layer_AAB.crt)

    # The future belongs to UTF-8.
    # Piping is used so widely in GNU toolchain that we have to apply UTF-8 manifest to all programs.
    if ver.min_os.major >= 6:
      subprocess.run([
        f'{ver.target}-gcc',
        '-std=c11',
        '-Os', '-c',
        paths.utf8_src_dir / 'utf8-init.c',
        '-o', build_dir / 'utf8-init.o',
      ], check = True)
      subprocess.run([
        f'{ver.target}-windres',
        '-O', 'coff',
        paths.utf8_src_dir / 'utf8-manifest.rc',
        '-o', build_dir / 'utf8-manifest.o',
      ], check = True)
      for crt_object in ['crt1.o', 'crt1u.o', 'crt2.o', 'crt2u.o']:
        subprocess.run([
          f'{ver.target}-gcc',
          '-r',
          build_dir / f'lib{ver.arch}' / crt_object,
          build_dir / 'utf8-init.o',
          build_dir / 'utf8-manifest.o',
          '-o', paths.layer_AAB.crt / 'usr/local' / ver.target / 'lib' / crt_object,
        ], check = True)

def _winpthreads(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',
  ]):
    build_dir = paths.src_dir.mingw_host / 'mingw-w64-libraries' / 'winpthreads' / 'build-AAB'
    ensure(build_dir)
    configure('winpthreads', build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      ),
    ])
    make_default('winpthreads', build_dir, config.jobs)

    # as the basis of gthread interface, it should be considered as part of gcc
    make_destdir_install('winpthreads', build_dir, paths.layer_AAB.gcc)

def _mcfgthread(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',
  ]):
    build_dir = paths.src_dir.mcfgthread / 'build-AAB'
    ensure(build_dir)

    ret = subprocess.run([
      'meson',
      'setup',
      '--cross-file', f'meson.cross.{ver.target}',
      build_dir,
    ], cwd = paths.src_dir.mcfgthread)
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

  # as the basis of gthread interface, it should be considered as part of gcc
  lib_dir = paths.layer_AAB.gcc / 'usr/local' / ver.target / 'lib'
  ensure(lib_dir)
  shutil.copy(build_dir / 'libmcfgthread.a', lib_dir / 'libmcfgthread.a')

  include_dir = paths.layer_AAB.gcc / 'usr/local' / ver.target / 'include' / 'mcfgthread'
  ensure(include_dir)
  header_files = [
    *paths.src_dir.mcfgthread.glob('mcfgthread/*.h'),
    *paths.src_dir.mcfgthread.glob('mcfgthread/*.hpp'),
    build_dir / 'version.h',
  ]
  for header_file in header_files:
    shutil.copy(header_file, include_dir / header_file.name)

def build_AAB_compiler(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _binutils(ver, paths, config)

  headers = _headers(ver, paths, config)
  headers.__next__()

  gcc = _gcc(ver, paths, config)
  gcc.__next__()

  _crt(ver, paths, config)

  _winpthreads(ver, paths, config)
  headers.__next__()

  if ver.thread == 'mcf':
    _mcfgthread(ver, paths, config)

  gcc.__next__()

def _gmp(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',
  ]):
    v = Version(ver.gmp)
    v_gcc = Version(ver.gcc)
    build_dir = paths.src_dir.gmp / 'build-AAB'
    ensure(build_dir)

    c_extra = []

    # GCC 15 defaults to C23, in which `foo()` means `foo(void)` instead of `foo(...)`.
    if v_gcc.major >= 15 and v < Version('6.4.0'):
      c_extra.append('-std=gnu11')

    configure('gmp', build_dir, [
      f'--prefix=/usr/local/{ver.target}',
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
    make_destdir_install('gmp', build_dir, paths.layer_AAB.gmp)

def _mpfr(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',

    paths.layer_AAB.gmp / 'usr/local',
  ]):
    build_dir = paths.src_dir.mpfr / 'build-AAB'
    ensure(build_dir)
    configure('mpfr', build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      ),
    ])
    make_default('mpfr', build_dir, config.jobs)
    make_destdir_install('mpfr', build_dir, paths.layer_AAB.mpfr)

def _mpc(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',

    paths.layer_AAB.gmp / 'usr/local',
    paths.layer_AAB.mpfr / 'usr/local',
  ]):
    build_dir = paths.src_dir.mpc / 'build-AAB'
    ensure(build_dir)
    configure('mpc', build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      ),
    ])
    make_default('mpc', build_dir, config.jobs)
    make_destdir_install('mpc', build_dir, paths.layer_AAB.mpc)

def _expat(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',
  ]):
    build_dir = paths.src_dir.expat / 'build-AAB'
    ensure(build_dir)
    configure('expat', build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      )
    ])
    make_default('expat', build_dir, config.jobs)
    make_destdir_install('expat', build_dir, paths.layer_AAB.expat)

def _iconv(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',
  ]):
    build_dir = paths.src_dir.iconv / 'build-AAB'
    ensure(build_dir)

    configure('iconv', build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--disable-nls',
      '--enable-static',
      '--disable-shared',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
      ),
    ])
    make_default('iconv', build_dir, config.jobs)
    make_destdir_install('iconv', build_dir, paths.layer_AAB.iconv)

def _intl(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.xmake / 'usr/local',

    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',
  ]):
    src_dir = paths.build_dir / 'intl'

    xmake_config('intl', src_dir, [
      '--plat=mingw',
      f'--arch={XMAKE_ARCH_MAP[ver.arch]}',
    ])
    xmake_build('intl', src_dir, config.jobs)

    install_dir = paths.layer_AAB.intl / 'usr/local' / ver.target
    xmake_install('intl', src_dir, install_dir)

def _pdcurses(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',
  ]):
    build_dir = paths.src_dir.pdcurses / 'wincon'
    make_custom('pdcurses', build_dir, [
      'pdcurses.a',
      f'CC={ver.target}-gcc',
      f'AR={ver.target}-ar',
      *cflags_B(
        c_extra = ['-I..', '-DPDC_WIDE'],
      ),
    ], config.jobs)

    lib_dir = paths.layer_AAB.pdcurses / 'usr/local' / ver.target / 'lib'
    ensure(lib_dir)
    shutil.copy(build_dir / 'pdcurses.a', lib_dir / 'libcurses.a')

    include_dir = paths.layer_AAB.pdcurses / 'usr/local' / ver.target / 'include'
    ensure(include_dir)
    shutil.copy(paths.src_dir.pdcurses / 'curses.h', include_dir / 'curses.h')

def _python(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.python / 'usr/local',
    paths.layer_AAA.xmake / 'usr/local',

    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.crt / 'usr/local',
  ]):
    src_dir = paths.src_dir.python

    config_args = []
    if ver.min_os.major < 6:
      config_args.append('--emulated-win-cv=1')

    xmake_config('python', src_dir, [
      '--plat=mingw',
      f'--arch={XMAKE_ARCH_MAP[ver.arch]}',
      *config_args,
    ])
    xmake_build('python', src_dir, config.jobs)

    install_dir = paths.layer_AAB.python / 'usr/local' / ver.target
    xmake_install('python', src_dir, install_dir, ['pythoncore'])

    stdlib_package_dir = src_dir / 'build/stdlib-package'
    ensure(stdlib_package_dir)
    xmake_install('python (stdlib)', src_dir, stdlib_package_dir, ['stdlib'])

    python_lib = stdlib_package_dir / 'Lib'
    shutil.copytree(f'/usr/local/share/gcc-{config.branch}/python', python_lib, dirs_exist_ok = True)
    subprocess.run([
      'python3', '-m', 'compileall',
      '-b',
      '-o', '2',
      '.',
    ], check = True, cwd = python_lib)

    python_lib_zip = install_dir / 'lib/python.zip'
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

  _expat(ver, paths, config)

  _iconv(ver, paths, config)

  _intl(ver, paths, config)

  _pdcurses(ver, paths, config)

  _python(ver, paths, config)
