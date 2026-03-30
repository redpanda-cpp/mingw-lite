import argparse
import logging
from packaging.version import Version
from pathlib import Path
import shutil
import subprocess

from module.alt_crt import postprocess_crt_import_libraries
from module.debug import shell_here
from module.path import ProjectPaths
from module.profile import BranchProfile
from module.util import XMAKE_ARCH_MAP, add_objects_to_static_lib, common_cross_layers, ensure, extract_shared_libs, overlayfs_ro, touch
from module.util import cflags_A, cflags_B, configure, make_custom, make_default, make_destdir_install
from module.util import meson_build, meson_config, meson_flags_B, meson_install
from module.util import xmake_build, xmake_config, xmake_install

def _binutils(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.src_dir.binutils / 'build-AAB'
  ensure(build_dir)
  configure(build_dir, [
    f'--prefix=/usr/local',
    f'--target={ver.target}',
    f'--build={config.build}',
    # prefer static
    '--disable-shared',
    '--enable-static',
    # features
    '--disable-install-libbfd',
    '--disable-multilib',
    '--disable-nls',
    *cflags_A(),
  ])
  make_default(build_dir, config.jobs)
  make_destdir_install(build_dir, paths.layer_AAB.binutils)

def _headers_1(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.src_dir.mingw / 'mingw-w64-headers' / 'build-AAB'
  ensure(build_dir)
  configure(build_dir, [
    f'--prefix=/usr/local/{ver.target}',
    f'--host={ver.target}',
    f'--build={config.build}',
    f'--with-default-msvcrt={ver.default_crt}',
    f'--with-default-win32-winnt=0x{ver.win32_winnt:04X}',
  ])
  make_default(build_dir, config.jobs)
  make_destdir_install(build_dir, paths.layer_AAB.headers)

def _headers_2(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  include_dir = paths.layer_AAB.headers / 'usr/local' / ver.target / 'include'
  for dummy_header in ['pthread_signal.h', 'pthread_time.h', 'pthread_unistd.h']:
    (include_dir / dummy_header).unlink()

def _gcc_1(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.src_dir.gcc / 'build-AAB'
  ensure(build_dir)

  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.gmp / 'usr/local',
    paths.layer_AAA.mpc / 'usr/local',
    paths.layer_AAA.mpfr / 'usr/local',

    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
  ]):
    config_flags = []

    if config.enable_shared:
      config_flags.append('--enable-shared')
    else:
      config_flags.append('--disable-shared')
    if ver.exception == 'dwarf':
      config_flags.append('--disable-sjlj-exceptions')
      config_flags.append('--with-dwarf2')
    if ver.fpmath:
      config_flags.append(f'--with-fpmath={ver.fpmath}')

    configure(build_dir, [
      f'--prefix=/usr/local',
      f'--libexecdir=/usr/local/lib',
      f'--with-gcc-major-version-only',
      f'--target={ver.target}',
      f'--build={config.build}',
      '--enable-static',
      # features
      '--disable-bootstrap',
      '--enable-checking=release',
      '--enable-host-pie',
      '--enable-languages=c,c++,fortran',
      '--disable-libgomp',
      '--disable-libmpx',
      '--disable-libstdcxx-pch',
      '--disable-multilib',
      '--disable-nls',
      f'--enable-threads={ver.thread}',
      # packages
      f'--with-arch={ver.march}',
      '--without-libcc1',
      '--with-tune=generic',
      *config_flags,
      *cflags_A(),
      *cflags_B('_FOR_TARGET',
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        optimize_for_size = ver.optimize_for_size,
      ),
    ])

    make_custom(build_dir, ['all-host'], config.jobs)
    make_custom(build_dir, [
      f'DESTDIR={paths.layer_AAB.gcc}',
      'install-host',
    ], jobs = 1)

def _gcc_2(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.src_dir.gcc / 'build-AAB'

  common_layers = [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.crt_target / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.mcfgthread / 'usr/local',
    paths.layer_AAB.mcfgthread_shared / 'usr/local',
    paths.layer_AAB.winpthreads / 'usr/local',
    paths.layer_AAB.winpthreads_shared / 'usr/local',
  ]

  with overlayfs_ro('/usr/local', common_layers):
    make_custom(build_dir, ['all-target-libgcc'], config.jobs)

  with overlayfs_ro('/usr/local', [
    # override crt
    paths.layer_AAB.atomic_bootstrap / 'usr/local',

    *common_layers,
  ]):
    make_custom(build_dir, ['all-target-libatomic'], config.jobs)

  with overlayfs_ro('/usr/local', common_layers):
    # target missing atomic instructions, and gcc will yield function calls.
    # here we add libatomic to libgcc, so we don't need to handle build flags later.
    if ver.march in ['i386', 'i486']:
      libgcc_a = build_dir / ver.target / 'libgcc/libgcc.a'
      atomic_objects = [*(build_dir / ver.target / 'libatomic').glob('*.o')]
      if ver.march == 'i386':
        sync_wrappers = ['sync_fetch_and_op', 'sync_op_and_fetch', 'sync_compare_and_swap']
        for wrapper in sync_wrappers:
          wrapper_src = paths.sync_src_dir / f'{wrapper}.cc'
          wrapper_obj = build_dir / f'{wrapper}.o'
          subprocess.run([
            f'{ver.target}-gcc', '-c', wrapper_src, '-o', wrapper_obj,
          ], check = True)
          atomic_objects.append(wrapper_obj)
      add_objects_to_static_lib(f'{ver.target}-ar', libgcc_a, atomic_objects)

    make_custom(build_dir, ['all-target'], config.jobs)
    make_custom(build_dir, [
      f'DESTDIR={paths.layer_AAB.gcc_lib}',
      'install-target',
    ], jobs = 1)

  base_prefix = paths.layer_AAB.gcc_lib / 'usr/local' / ver.target
  shared_prefix = paths.layer_AAB.gcc_lib_shared / 'usr/local' / ver.target
  if config.enable_shared:
    extract_shared_libs(base_prefix, shared_prefix, [
      'lib/libgcc_s.a',  # shared libgcc
    ])
  else:
    touch(shared_prefix / '.keep')

def _crt_base(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.src_dir.mingw / 'mingw-w64-crt' / 'build-AAB'

  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
  ]):
    ensure(build_dir)

    multilib_flags = [
      '--enable-lib64' if ver.arch == '64' else '--disable-lib64',
      '--enable-libarm64' if ver.arch == 'arm64' else '--disable-libarm64',
      '--enable-lib32' if ver.arch == '32' else '--disable-lib32',
      '--disable-libarm32',
    ]

    configure(build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      f'--with-default-msvcrt={ver.default_crt}',
      f'--with-default-win32-winnt=0x{ver.win32_winnt:04X}',
      *multilib_flags,
      *cflags_B(optimize_for_size = ver.optimize_for_size),
      # create modern (short) import libraries
      # https://github.com/mingw-w64/mingw-w64/issues/149
      'DLLTOOL=llvm-dlltool',
      'AR=llvm-ar',
      'RANLIB=llvm-ranlib',
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_AAB.crt_base)

def _crt_host(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.mingw)

  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.xmake / 'usr/local',

    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.crt_base / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
  ]):
    thunk_src_dir = paths.in_tree_src_dir.thunk

    if ver.utf8_thunk:
      thunk_profile = 'toolchain-utf8'
    else:
      thunk_profile = 'toolchain'

    xmake_config(thunk_src_dir, [
      '--builddir=build-AAB',
      '--plat=mingw',
      f'--arch={XMAKE_ARCH_MAP[ver.arch]}',
      '--mingw=/usr/local',
      f'--mingw-version={v.major}',
      f'--thunk-level={ver.min_os}',
      f'--profile={thunk_profile}',
    ])
    xmake_build(thunk_src_dir, config.jobs)
    xmake_install(thunk_src_dir, paths.layer_AAB.thunk_host / 'usr/local' / ver.target)

    # Post-process import libraries to handle weak symbol aliases
    # llvm-dlltool uses weak symbols for aliases which binutils ld doesn't handle well
    # We split them into normal symbols (llvm-dlltool) and aliases (binutils dlltool)
    thunk_lib_dir = paths.layer_AAB.thunk_host / 'usr/local' / ver.target / 'lib'
    crt0_lib_dir = paths.layer_AAB.crt_base / 'usr/local' / ver.target / 'lib'
    crt_lib_dir = paths.layer_AAB.crt_host / 'usr/local' / ver.target / 'lib'
    postprocess_crt_import_libraries(
      ver,
      thunk_lib_dir,
      crt0_lib_dir,
      crt_lib_dir,
      assert_thunk_free = False,
      jobs = config.jobs,
    )

    # special handling libmsvcrt.a
    if ver.default_crt == 'msvcrt':
      shutil.copy(crt_lib_dir / 'libmsvcrt-os.a', crt_lib_dir / 'libmsvcrt.a')
    if ver.default_crt == 'ucrt':
      shutil.copy(crt_lib_dir / 'libucrt.a', crt_lib_dir / 'libmsvcrt.a')

    crt0_inc_dir = paths.layer_AAB.crt_base / 'usr/local' / ver.target / 'include'
    crt_inc_dir = paths.layer_AAB.crt_host / 'usr/local' / ver.target / 'include'
    shutil.copytree(crt0_inc_dir, crt_inc_dir, dirs_exist_ok = True)

def _crt_target(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.mingw)

  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.xmake / 'usr/local',

    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.crt_base / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
  ]):
    thunk_src_dir = paths.in_tree_src_dir.thunk

    xmake_config(thunk_src_dir, [
      '--builddir=build-AAB',
      '--plat=mingw',
      f'--arch={XMAKE_ARCH_MAP[ver.arch]}',
      '--mingw=/usr/local',
      f'--mingw-version={v.major}',
      f'--thunk-level={ver.min_os}',
      '--profile=core',
    ])
    xmake_build(thunk_src_dir, config.jobs)
    xmake_install(thunk_src_dir, paths.layer_AAB.thunk_target / 'usr/local' / ver.target)

    # Post-process import libraries to handle weak symbol aliases
    # llvm-dlltool uses weak symbols for aliases which binutils ld doesn't handle well
    # We split them into normal symbols (llvm-dlltool) and aliases (binutils dlltool)
    thunk_lib_dir = paths.layer_AAB.thunk_target / 'usr/local' / ver.target / 'lib'
    crt0_lib_dir = paths.layer_AAB.crt_base / 'usr/local' / ver.target / 'lib'
    crt_lib_dir = paths.layer_AAB.crt_target / 'usr/local' / ver.target / 'lib'
    postprocess_crt_import_libraries(
      ver,
      thunk_lib_dir,
      crt0_lib_dir,
      crt_lib_dir,
      assert_thunk_free = ver.thunk_free,
      jobs = config.jobs,
    )

    # special handling libmsvcrt.a
    if ver.default_crt == 'msvcrt':
      shutil.copy(crt_lib_dir / 'libmsvcrt-os.a', crt_lib_dir / 'libmsvcrt.a')
    if ver.default_crt == 'ucrt':
      shutil.copy(crt_lib_dir / 'libucrt.a', crt_lib_dir / 'libmsvcrt.a')

    crt0_inc_dir = paths.layer_AAB.crt_base / 'usr/local' / ver.target / 'include'
    crt_inc_dir = paths.layer_AAB.crt_target / 'usr/local' / ver.target / 'include'
    shutil.copytree(crt0_inc_dir, crt_inc_dir, dirs_exist_ok = True)

def _utf8(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.build_dir / 'utf8'
  ensure(build_dir)
  obj_dir = paths.layer_AAB.utf8 / 'usr/local' / ver.target / 'lib'
  ensure(obj_dir)

  # The future belongs to UTF-8.
  # Piping is used so widely in GNU toolchain that we have to apply UTF-8 manifest to all programs.
  # Linking UTF-8 objects (manifest and console hacks) to CRT init objects is an efficient way.
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.crt_base / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
  ]):
    subprocess.run([
      f'{ver.target}-gcc',
      '-std=c11',
      '-Os', '-c',
      paths.utf8_src_dir / 'console-mode-hack.c',
      '-o', build_dir / 'console-mode-hack.o',
    ], check = True)
    startup_adds = [build_dir / 'console-mode-hack.o']

    if ver.utf8_thunk:
      # New method: UTF-8 thunks for Win32 and CRT
      # Most things has been done in thunks. What we need is console mode hack object.
      pass
    else:
      # Old method: UTF-8 manifest
      obj_dir = paths.layer_AAB.utf8 / 'usr/local' / ver.target / 'lib'
      ensure(obj_dir)

      subprocess.run([
        f'{ver.target}-gcc',
        '-std=c11',
        '-Os', '-c',
        paths.utf8_src_dir / 'console-code-page-hack.c',
        '-o', build_dir / 'console-code-page-hack.o',
      ], check = True)
      startup_adds.append(build_dir / 'console-code-page-hack.o')
      subprocess.run([
        f'{ver.target}-windres',
        '-O', 'coff',
        paths.utf8_src_dir / 'utf8-manifest.rc',
        '-o', build_dir / 'utf8-manifest.o',
      ], check = True)
      startup_adds.append(build_dir / 'utf8-manifest.o')

    for crt_object in ['crt2.o', 'crt2u.o']:
      subprocess.run([
        f'{ver.target}-gcc',
        '-r',
        paths.layer_AAB.crt_base / 'usr/local' / ver.target / 'lib' / crt_object,
        *startup_adds,
        '-o', obj_dir / crt_object,
      ], check = True)

def _atomic_bootstrap(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  lib_dir = paths.layer_AAB.atomic_bootstrap / 'usr/local' / ver.target / 'lib'
  ensure(lib_dir)

  if ver.march != 'i386':
    return

  build_dir = paths.build_dir / 'atomic_bootstrap'
  ensure(build_dir)

  # bootstrapping i386 libatomic is tricky.
  # the configure script checks whether GCC can compile program.
  # however, CRT startup code depends on libatomic, which is not built yet.
  # here we provide fake symbols to pass the check.

  # also we cannot handle it in gcc phases (adding to libgcc.a),
  # because it will be overwritten by 'configure-target-libatomic'.
  # here we choose libkernel32.a.
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.crt_base / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
  ]):
    fake_object = build_dir / 'libatomic_fake.o'
    subprocess.run([
      f'{ver.target}-gcc',
      '-c', paths.sync_src_dir / 'bootstrap.c',
      '-o', fake_object,
    ], check = True)

    crt_kernel32 = paths.layer_AAB.crt_host / 'usr/local' / ver.target / 'lib/libkernel32.a'
    atomic_kernel32 = lib_dir / 'libkernel32.a'
    shutil.copy(crt_kernel32, atomic_kernel32)
    add_objects_to_static_lib(
      'llvm-ar',
      atomic_kernel32,
      [fake_object],
    )

def _mcfgthread(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  base_prefix = paths.layer_AAB.mcfgthread / 'usr/local' / ver.target
  shared_prefix = paths.layer_AAB.mcfgthread_shared / 'usr/local' / ver.target

  if ver.thread != 'mcf':
    touch(base_prefix / '.keep')
    touch(shared_prefix / '.keep')
    return

  v = Version(ver.mcfgthread.replace('-ga', ''))
  build_dir = 'build-AAB'

  if v >= Version('2.2'):
    cross_file = f'cross/gcc.{ver.target}'
  else:
    cross_file = f'meson.cross.{ver.target}'

  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.python / 'usr/local',
    paths.layer_AAA.meson / 'usr/local',

    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.crt_target / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
  ]):
    meson_config(
      paths.src_dir.mcfgthread,
      extra_args = [
        f'--prefix=/usr/local/{ver.target}',
        '--cross-file', cross_file,
        *meson_flags_B(
          cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
          optimize_for_size = ver.optimize_for_size,
        ),
      ],
      build_dir = build_dir,
    )
    meson_build(
      paths.src_dir.mcfgthread,
      jobs = config.jobs,
      build_dir = build_dir,
    )
    meson_install(
      paths.src_dir.mcfgthread,
      destdir = paths.layer_AAB.mcfgthread,
      build_dir = build_dir,
    )

  if config.enable_shared:
    extract_shared_libs(base_prefix, shared_prefix)
  else:
    touch(shared_prefix / '.keep')
    extract_shared_libs(base_prefix, None)

def _winpthreads(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.binutils / 'usr/local',
    paths.layer_AAB.crt_target / 'usr/local',
    paths.layer_AAB.gcc / 'usr/local',
    paths.layer_AAB.headers / 'usr/local',
    paths.layer_AAB.mcfgthread / 'usr/local',
    paths.layer_AAB.mcfgthread_shared / 'usr/local',
  ]):
    build_dir = paths.src_dir.mingw / 'mingw-w64-libraries' / 'winpthreads' / 'build-AAB'
    ensure(build_dir)
    configure(build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-shared' if config.enable_shared else '--disable-shared',
      '--enable-static',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        optimize_for_size = ver.optimize_for_size,
      ),
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_AAB.winpthreads)

  base_prefix = paths.layer_AAB.winpthreads / 'usr/local' / ver.target
  shared_prefix = paths.layer_AAB.winpthreads_shared / 'usr/local' / ver.target
  if config.enable_shared:
    extract_shared_libs(base_prefix, shared_prefix)
  else:
    touch(shared_prefix / '.keep')

def build_AAB_compiler(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _binutils(ver, paths, config)
  _headers_1(ver, paths, config)
  _gcc_1(ver, paths, config)
  _crt_base(ver, paths, config)
  _crt_host(ver, paths, config)
  _crt_target(ver, paths, config)
  _utf8(ver, paths, config)
  _atomic_bootstrap(ver, paths, config)
  _mcfgthread(ver, paths, config)
  _winpthreads(ver, paths, config)
  _headers_2(ver, paths, config)
  _gcc_2(ver, paths, config)

def _gmp(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),
  ]):
    v = Version(ver.gmp)
    v_gcc = Version(ver.gcc)
    build_dir = paths.src_dir.gmp / 'build-AAB'
    ensure(build_dir)

    c_extra = []

    # GCC 15 defaults to C23, in which `foo()` means `foo(void)` instead of `foo(...)`.
    if v_gcc.major >= 15 and v < Version('6.4.0'):
      c_extra.append('-std=gnu11')

    configure(build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--disable-assembly',
      '--enable-static',
      '--disable-shared',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        c_extra = c_extra,
        optimize_for_size = ver.optimize_for_size,
      ),
      # To determine build system compiler, the configure script will firstly try host
      # compiler (i.e. *-w64-mingw32-gcc) and check whether the output is executable
      # (and fallback to cc otherwise). However, in WSL or Linux with Wine configured,
      # the check passes and thus *-w64-mingw32-gcc is detected as build system compiler.
      # Here we force the build system compiler to be gcc.
      'CC_FOR_BUILD=gcc',
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_AAB.gmp)

def _mpfr(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),

    paths.layer_AAB.gmp / 'usr/local',
  ]):
    build_dir = paths.src_dir.mpfr / 'build-AAB'
    ensure(build_dir)
    configure(build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        optimize_for_size = ver.optimize_for_size,
      ),
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_AAB.mpfr)

def _mpc(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),

    paths.layer_AAB.gmp / 'usr/local',
    paths.layer_AAB.mpfr / 'usr/local',
  ]):
    build_dir = paths.src_dir.mpc / 'build-AAB'
    ensure(build_dir)
    configure(build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        optimize_for_size = ver.optimize_for_size,
      ),
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_AAB.mpc)

def _expat(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),
  ]):
    build_dir = paths.src_dir.expat / 'build-AAB'
    ensure(build_dir)
    configure(build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-static',
      '--disable-shared',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        optimize_for_size = ver.optimize_for_size,
      )
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_AAB.expat)

def _iconv(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),
  ]):
    build_dir = paths.src_dir.iconv / 'build-AAB'
    ensure(build_dir)

    configure(build_dir, [
      f'--prefix=/usr/local/{ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--disable-nls',
      '--enable-static',
      '--disable-shared',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        optimize_for_size = ver.optimize_for_size,
      ),
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_AAB.iconv)

def _intl(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.xmake / 'usr/local',

    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),
  ]):
    src_dir = paths.build_dir / 'intl'

    xmake_config(src_dir, [
      '--plat=mingw',
      f'--arch={XMAKE_ARCH_MAP[ver.arch]}',
      '--mingw=/usr/local',
    ])
    xmake_build(src_dir, config.jobs)

    install_dir = paths.layer_AAB.intl / 'usr/local' / ver.target
    xmake_install(src_dir, install_dir)

def _pdcurses(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),
  ]):
    build_dir = paths.src_dir.pdcurses / 'wincon'
    make_custom(build_dir, [
      'pdcurses.a',
      f'CC={ver.target}-gcc',
      f'AR={ver.target}-ar',
      *cflags_B(
        c_extra = ['-I..', '-DPDC_WIDE'],
        optimize_for_size = ver.optimize_for_size,
      ),
    ], config.jobs)

    lib_dir = paths.layer_AAB.pdcurses / 'usr/local' / ver.target / 'lib'
    ensure(lib_dir)
    shutil.copy(build_dir / 'pdcurses.a', lib_dir / 'libcurses.a')

    include_dir = paths.layer_AAB.pdcurses / 'usr/local' / ver.target / 'include'
    ensure(include_dir)
    shutil.copy(paths.src_dir.pdcurses / 'curses.h', include_dir / 'curses.h')

def _python(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v_gcc = Version(ver.gcc)

  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.python / 'usr/local',
    paths.layer_AAA.xmake / 'usr/local',

    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),
  ]):
    src_dir = paths.src_dir.python

    config_args = []
    if ver.min_os.major < 6:
      config_args.append('--emulated-win-cv=1')

    xmake_config(src_dir, [
      '--plat=mingw',
      f'--arch={XMAKE_ARCH_MAP[ver.arch]}',
      '--mingw=/usr/local',
      *config_args,
    ])
    xmake_build(src_dir, config.jobs)

    install_dir = paths.layer_AAB.python / 'usr/local' / ver.target
    xmake_install(src_dir, install_dir, ['pythoncore'])

    stdlib_package_dir = src_dir / 'build/stdlib-package'
    ensure(stdlib_package_dir)
    xmake_install(src_dir, stdlib_package_dir, ['stdlib'])

    python_lib = stdlib_package_dir / 'Lib'
    shutil.copytree(f'/usr/local/share/gcc-{v_gcc.major}/python', python_lib, dirs_exist_ok = True)
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
