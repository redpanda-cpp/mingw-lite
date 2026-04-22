import argparse
import logging
import os
from packaging.version import Version
from pathlib import Path
import re
import shutil
import subprocess
from typing import List, Optional

from module.alt_crt import postprocess_crt_import_libraries
from module.debug import shell_here
from module.path import ProjectPaths
from module.profile import BranchProfile
from module.util import XMAKE_ARCH_MAP, add_objects_to_static_lib, common_cross_layers, ensure, extract_shared_libs, overlayfs_ro, remove_info_main_menu
from module.util import cflags_B, configure, make_custom, make_default, make_destdir_install
from module.util import cmake_build, cmake_config, cmake_flags_B, cmake_install
from module.util import meson_build, meson_config, meson_flags_B, meson_install
from module.util import xmake_build, xmake_config, xmake_install

def build_ABB_test_driver(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  ensure(paths.layer_ABB.test_driver)

  mingw_dir = paths.sat_mingw_dir.relative_to(paths.sat_dir)
  xmake_arch = XMAKE_ARCH_MAP[ver.arch]
  debug_build_dir = f'build/mingw/{xmake_arch}/debug'
  release_build_dir = f'build/mingw/{xmake_arch}/release'

  flags = [
    '-std=c11', '-O2', '-municode', '-s',
    f'-DMINGW_DIR="{mingw_dir}"',
    f'-DSHARED_DIR=L"{paths.shared_dir}"',
    f'-DXMAKE_ARCH="{xmake_arch}"',
    f'-DDEBUG_BUILD_DIR="{debug_build_dir}"',
    f'-DRELEASE_BUILD_DIR="{release_build_dir}"',
  ]
  if ver.utf8_thunk:
    flags.append('-DENABLE_UTF8')

  with overlayfs_ro('/usr/local', [
    # override CRT
    paths.layer_AAB.utf8 / 'usr/local',

    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),
  ]):
    gcc_exe = f'{ver.target}-gcc'

    src_dir = paths.root_dir / 'support/sat'
    pkg_dir = paths.layer_ABB.test_driver
    common_c = src_dir / 'common.c'

    subprocess.run([
      gcc_exe,
      *flags,
      src_dir / 'set-path.c', common_c,
      '-o', pkg_dir / 'set-path.exe',
    ], check = True)

    subprocess.run([
      gcc_exe,
      *flags,
      src_dir / 'test-compiler.c', common_c,
      '-o', pkg_dir / 'test-compiler.exe',
    ], check = True)

    if ver.utf8_user_crt:
      subprocess.run([
        gcc_exe,
        *flags,
        src_dir / 'test-console.c', common_c,
        '-o', pkg_dir / 'test-console.exe',
      ], check = True)

    subprocess.run([
      gcc_exe,
      *flags,
      src_dir / 'test-make-gdb.c', common_c,
      '-o', pkg_dir / 'test-make-gdb.exe',
    ], check = True)

    subprocess.run([
      gcc_exe,
      *flags,
      src_dir / 'test-shared.c', common_c,
      '-o', pkg_dir / 'test-shared.exe',
    ], check = True)

def _binutils(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    # override CRT
    paths.layer_AAB.utf8 / 'usr/local',

    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),

    paths.layer_AAB.intl / 'usr/local',
  ]):
    build_dir = paths.src_dir.binutils / 'build-ABB'
    ensure(build_dir)

    configure(build_dir, [
      '--prefix=',
      f'--host={ver.target}',
      f'--target={ver.target}',
      f'--build={config.build}',
      # workaround: bfd plugin 'dep' should be built as shared object
      '--enable-shared',
      '--enable-static',
      # features
      '--disable-install-libbfd',
      '--disable-multilib',
      '--enable-nls',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        optimize_for_speed = ver.opt_speed,
        lto = ver.profile_opt_lto,
      ),
      f'AR={ver.target}-gcc-ar',
      f'RANLIB={ver.target}-gcc-ranlib',
    ])
    make_default(build_dir, config.jobs)
    make_custom(build_dir, [
      f'DESTDIR={paths.layer_ABB.binutils}',
      # use native layout
      'tooldir=',
      'install',
    ], jobs = 1)

  remove_info_main_menu(paths.layer_ABB.binutils)

  license_dir = paths.layer_ABB.binutils / 'share/licenses/binutils'
  ensure(license_dir)
  for file in ['COPYING', 'COPYING3', 'COPYING.LIB', 'COPYING3.LIB']:
    shutil.copy(paths.src_dir.binutils / file, license_dir / file)

def _headers(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  build_dir = paths.src_dir.mingw / 'mingw-w64-headers' / 'build-ABB'
  ensure(build_dir)
  configure(build_dir, [
    '--prefix=',
    f'--host={ver.target}',
    f'--build={config.build}',
    f'--with-default-msvcrt={ver.default_crt}',
    f'--with-default-win32-winnt=0x{ver.win32_winnt:04X}',
  ])
  make_default(build_dir, config.jobs)
  make_destdir_install(build_dir, paths.layer_ABB.headers)

  include_dir = paths.layer_ABB.headers / 'include'
  for dummy_header in ['pthread_signal.h', 'pthread_time.h', 'pthread_unistd.h']:
    (include_dir / dummy_header).unlink()

  license_dir = paths.layer_ABB.headers / 'share/licenses/headers'
  ensure(license_dir)
  shutil.copy(paths.src_dir.mingw / 'COPYING', license_dir / 'COPYING')

def _crt0(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', common_cross_layers(paths)):
    build_dir = paths.src_dir.mingw / 'mingw-w64-crt' / 'build-ABB'
    ensure(build_dir)

    multilib_flags = [
      '--enable-lib64' if ver.arch == '64' else '--disable-lib64',
      '--enable-libarm64' if ver.arch == 'arm64' else '--disable-libarm64',
      '--enable-lib32' if ver.arch == '32' else '--disable-lib32',
      '--disable-libarm32',
    ]

    configure(build_dir, [
      '--prefix=',
      f'--host={ver.target}',
      f'--build={config.build}',
      f'--with-default-msvcrt={ver.default_crt}',
      f'--with-default-win32-winnt=0x{max(ver.win32_winnt, 0x0400):04X}',
      *multilib_flags,
      *cflags_B(optimize_for_speed = ver.opt_speed),
      # create modern (short) import libraries
      # https://github.com/mingw-w64/mingw-w64/issues/149
      'DLLTOOL=llvm-dlltool',
      'AR=llvm-ar',
      'RANLIB=llvm-ranlib',
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_ABB.crt0)

def _crt(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.mingw)

  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.xmake / 'usr/local',

    paths.layer_AAB.crt_base / 'usr/local',
    *common_cross_layers(paths),

    # u8crt
    paths.layer_AAB.crt_shared / 'usr/local',
    paths.layer_AAB.gcc_lib_shared / 'usr/local',
    paths.layer_AAB.winpthreads_shared / 'usr/local',
  ]):
    thunk_src_dir = paths.in_tree_src_dir.thunk

    config_flags: List[str] = []
    if ver.utf8_user_crt:
      config_flags.append('--u8crt=y')

    xmake_config(thunk_src_dir, [
      '--buildir=build-ABB',
      '--plat=mingw',
      f'--arch={XMAKE_ARCH_MAP[ver.arch]}',
      '--mingw=/usr/local',
      f'--mingw-version={v.major}',
      f'--thunk-level={ver.min_os}',
      '--profile=core',
      *config_flags,
    ])
    xmake_build(thunk_src_dir, config.jobs)
    xmake_install(thunk_src_dir, paths.layer_ABB.thunk)

    thunk_lib_dir = paths.layer_ABB.thunk / 'lib'
    crt0_lib_dir = paths.layer_ABB.crt0 / 'lib'
    crt_lib_dir = paths.layer_ABB.crt / 'lib'

    # u8crt
    if ver.utf8_user_crt:
      xmake_install(thunk_src_dir, paths.layer_ABB.crt, ['utf8-musl.a'])
      xmake_install(thunk_src_dir, paths.layer_ABB.crt_shared, ['utf8-musl.so'])

      # trigger post-processing import libraries
      shutil.copy(crt0_lib_dir / 'libucrt.a', crt0_lib_dir / 'libutf8-ucrt.a')

      # -mutf8 addition
      subprocess.run([
        f'{ver.target}-gcc', '-std=c11', '-Os', '-c',
        paths.utf8_src_dir / 'utf8-console.c',
        '-o', crt_lib_dir / 'utf8-console.o',
      ], check = True)
      subprocess.run([
        f'{ver.target}-windres', '-O', 'coff',
        paths.utf8_src_dir / 'utf8-manifest.rc',
        '-o', crt_lib_dir / 'utf8-manifest.o',
      ], check = True)

    # Post-process import libraries to handle weak symbol aliases
    # llvm-dlltool uses weak symbols for aliases which binutils ld doesn't handle well
    # We split them into normal symbols (llvm-dlltool) and aliases (binutils dlltool)
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

    crt0_inc_dir = paths.layer_ABB.crt0 / 'include'
    crt_inc_dir = paths.layer_ABB.crt / 'include'
    shutil.copytree(crt0_inc_dir, crt_inc_dir, dirs_exist_ok = True)

  license_dir = paths.layer_ABB.crt / 'share/licenses/crt'
  ensure(license_dir)
  shutil.copy(paths.src_dir.mingw / 'COPYING', license_dir / 'COPYING')

def _winpthreads(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.crt_target / 'usr/local',
    paths.layer_AAB.mcfgthread_shared / 'usr/local',
    *common_cross_layers(paths),
  ]):
    build_dir = paths.src_dir.mingw / 'mingw-w64-libraries' / 'winpthreads' / 'build-ABB'
    ensure(build_dir)
    configure(build_dir, [
      '--prefix=',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-shared',
      '--enable-static',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        optimize_for_speed = ver.opt_speed,
      ),
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_ABB.winpthreads)

  base_prefix = paths.layer_ABB.winpthreads
  shared_prefix = paths.layer_ABB.winpthreads_shared
  extract_shared_libs(base_prefix, shared_prefix)

  license_dir = paths.layer_ABB.winpthreads / 'share/licenses/winpthreads'
  ensure(license_dir)
  shutil.copy(paths.src_dir.mingw / 'mingw-w64-libraries/winpthreads/COPYING', license_dir / 'COPYING')

def _mcfgthread(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  base_prefix = paths.layer_ABB.mcfgthread
  shared_prefix = paths.layer_ABB.mcfgthread_shared

  if ver.thread != 'mcf':
    ensure(base_prefix)
    return

  v = Version(ver.mcfgthread.replace('-ga', ''))
  build_dir = 'build-ABB'

  if v >= Version('2.2'):
    cross_file = f'cross/gcc.{ver.target}'
  else:
    cross_file = f'meson.cross.{ver.target}'

  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.python / 'usr/local',
    paths.layer_AAA.meson / 'usr/local',

    paths.layer_AAB.crt_target / 'usr/local',
    *common_cross_layers(paths),
  ]):
    meson_config(
      paths.src_dir.mcfgthread,
      extra_args = [
        '--cross-file', cross_file,
        '--prefix', '/',
        *meson_flags_B(
          cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
          optimize_for_speed = ver.opt_speed,
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
      destdir = paths.layer_ABB.mcfgthread,
      build_dir = build_dir,
    )

  extract_shared_libs(base_prefix, shared_prefix)

  license_dir = paths.layer_AAB.gcc / 'share/licenses/mcfgthread'
  ensure(license_dir)
  shutil.copy(paths.src_dir.mcfgthread / 'LICENSE.TXT', license_dir / 'LICENSE.TXT')
  for file in ['gcc-exception-3.1.txt', 'gpl-3.0.txt', 'lgpl-3.0.txt']:
    shutil.copy(paths.src_dir.mcfgthread / 'licenses' / file, license_dir / file)

def _nowide(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  base_prefix = paths.layer_ABB.nowide
  shared_prefix = paths.layer_ABB.nowide_shared

  if not ver.utf8_user_crt:
    ensure(base_prefix)
    return

  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.crt_target / 'usr/local',
    *common_cross_layers(paths),
  ]):
    build_dir = 'build-ABB'
    cmake_config(
      paths.src_dir.nowide,
      extra_args = [
        f'-DCMAKE_TOOLCHAIN_FILE={paths.cmake_cross_file}',
        '-DCMAKE_INSTALL_PREFIX=',
        '-DBUILD_SHARED_LIBS=OFF',
        *cmake_flags_B(
          optimize_for_speed = ver.opt_speed,
        ),
      ],
      build_dir = build_dir,
    )
    cmake_build(
      paths.src_dir.nowide,
      jobs = config.jobs,
      build_dir = build_dir,
    )
    cmake_install(
      paths.src_dir.nowide,
      destdir = paths.layer_ABB.nowide,
      build_dir = build_dir,
    )

  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.crt_target / 'usr/local',
    paths.layer_AAB.crt_shared / 'usr/local',
    paths.layer_AAB.gcc_lib_shared / 'usr/local',
    paths.layer_AAB.mcfgthread_shared / 'usr/local',
    paths.layer_AAB.winpthreads_shared / 'usr/local',
    *common_cross_layers(paths),
  ]):
    build_dir = 'build-ABB-shared'
    cmake_config(
      paths.src_dir.nowide,
      extra_args = [
        f'-DCMAKE_TOOLCHAIN_FILE={paths.cmake_cross_file}',
        '-DCMAKE_INSTALL_PREFIX=',
        '-DBUILD_SHARED_LIBS=ON',
        *cmake_flags_B(
          optimize_for_speed = ver.opt_speed,
        ),
      ],
      build_dir = build_dir,
    )
    cmake_build(
      paths.src_dir.nowide,
      jobs = config.jobs,
      build_dir = build_dir,
    )
    cmake_install(
      paths.src_dir.nowide,
      destdir = paths.layer_ABB.nowide,
      build_dir = build_dir,
    )

  extract_shared_libs(base_prefix, shared_prefix)

  license_dir = paths.layer_ABB.nowide / 'share/licenses/nowide'
  ensure(license_dir)
  shutil.copy(paths.src_dir.nowide / 'LICENSE', license_dir / 'LICENSE')

def _gcc_1(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.gcc)

  build_dir = paths.src_dir.gcc / 'build-ABB'
  ensure(build_dir)

  with overlayfs_ro('/usr/local', [
    # override CRT
    paths.layer_AAB.utf8 / 'usr/local',

    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),

    paths.layer_AAB.gmp / 'usr/local',
    paths.layer_AAB.mpfr / 'usr/local',
    paths.layer_AAB.mpc / 'usr/local',
    paths.layer_AAB.iconv / 'usr/local',
    paths.layer_AAB.intl / 'usr/local',
  ]):
    config_flags: List[str] = []

    if ver.exception == 'dwarf':
      config_flags.append('--disable-sjlj-exceptions')
      config_flags.append('--with-dwarf2')
    if ver.fpmath:
      config_flags.append(f'--with-fpmath={ver.fpmath}')
    if ver.utf8_thunk:
      config_flags.append('--disable-win32-utf8-manifest')
    if v.major >= 16:
      config_flags.append('--enable-tls')

    configure(build_dir, [
      '--prefix=',
      '--libexecdir=/lib',
      f'--with-gcc-major-version-only',
      f'--target={ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-shared',
      '--enable-static',
      # features
      '--disable-bootstrap',
      '--enable-checking=release',
      '--enable-languages=c,c++',
      '--enable-libgomp',
      '--disable-libmpx',
      '--disable-libstdcxx-pch',
      '--disable-multilib',
      '--enable-nls',
      f'--enable-threads={ver.thread}',
      '--disable-win32-registry',
      # packages
      f'--with-arch={ver.march}',
      '--without-libcc1',
      '--with-libiconv',
      '--with-tune=generic',
      *config_flags,
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        optimize_for_speed = ver.opt_speed,
        lto = ver.profile_opt_lto,
      ),
      *cflags_B('_FOR_TARGET',
        # CPPFLAGS_FOR_TARGET is not passed
        common_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        optimize_for_speed = ver.opt_speed,
      ),
      f'AR={ver.target}-gcc-ar',
      f'RANLIB={ver.target}-gcc-ranlib',
    ])
    make_custom(build_dir, ['all-host'], config.jobs)
    make_custom(build_dir, [
      f'DESTDIR={paths.layer_ABB.gcc}',
      'install-host',
    ], jobs = 1)

def _gcc_2(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  v = Version(ver.gcc)
  build_dir = paths.src_dir.gcc / 'build-ABB'

  with overlayfs_ro('/usr/local', [
    paths.layer_AAB.crt_target / 'usr/local',
    paths.layer_AAB.gcc_lib_shared / 'usr/local',
    paths.layer_AAB.mcfgthread_shared / 'usr/local',
    paths.layer_AAB.winpthreads_shared / 'usr/local',
    *common_cross_layers(paths),
  ]), overlayfs_ro(f'/{ver.target}/include', [
    # the build system expects target headers at `/$triplet/include`.
    # it's generally okay if that directory does not exists,
    # because cross toolchain is complete and provides all headers.

    # but when building libstdc++ std module,
    # gcc's <fenv.h> wrapper #include_next <fenv.h>,
    # and the compiler founds cross gcc's <fenv.h> wrapper,
    # and that file is omitted because of same include guard,
    # and thus the real <fenv.h> will never be included.
    f'/usr/local/{ver.target}/include',
  ]):
    make_custom(build_dir, ['all-target'], config.jobs)
    make_custom(build_dir, [
      f'DESTDIR={paths.layer_ABB.gcc_lib}',
      'install-target',
    ], jobs = 1)

    # add `print.o` to `libstdc++.a`, allowing `<print>` without `-lstdc++exp`
    # it's okay since we only keep ABI stable in a major version
    if 14 <= v.major < 17:
      add_objects_to_static_lib(f'{ver.target}-ar',
        paths.layer_ABB.gcc_lib / 'lib/libstdc++.a',
        [build_dir / ver.target / 'libstdc++-v3/src/c++23/print.o'],
      )

  base_prefix = paths.layer_ABB.gcc_lib
  shared_prefix = paths.layer_ABB.gcc_lib_shared
  extract_shared_libs(base_prefix, shared_prefix, [
    'lib/libgcc_s.a',  # shared libgcc
  ])

  remove_info_main_menu(paths.layer_ABB.gcc)

  license_dir = paths.layer_ABB.gcc / 'share/licenses/gcc'
  ensure(license_dir)
  for file in ['COPYING', 'COPYING3', 'COPYING.RUNTIME', 'COPYING.LIB', 'COPYING3.LIB']:
    shutil.copy(paths.src_dir.gcc / file, license_dir / file)

def _gdb(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    # override CRT
    paths.layer_AAB.utf8 / 'usr/local',

    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),

    paths.layer_AAB.expat / 'usr/local',
    paths.layer_AAB.gmp / 'usr/local',
    paths.layer_AAB.mpfr / 'usr/local',
    paths.layer_AAB.mpc / 'usr/local',
    paths.layer_AAB.iconv / 'usr/local',
    paths.layer_AAB.pdcurses / 'usr/local',
    paths.layer_AAB.python / 'usr/local',
  ]):
    build_dir = paths.src_dir.gdb / 'build-ABB'
    ensure(build_dir)

    cflags = cflags_B(
      cpp_extra = [
        f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}',
        '-DPDC_WIDE',
        # workaround: bfd and gnulib disagree about i686 time_t
        '-D__MINGW_USE_VC2005_COMPAT',
      ],
      optimize_for_speed = ver.opt_speed,
    )

    configure(build_dir, [
      '--prefix=',
      f'--target={ver.target}',
      f'--host={ver.target}',
      f'--build={config.build}',
      # features
      '--disable-install-libbfd',
      # explicitly disable nls to prevent libbfd from installing locale files,
      # which collide with binutils.
      # there is no nls support in gdb.
      '--disable-nls',
      '--enable-tui',
      # packages
      f'--with-python=/usr/local/{ver.target}/python-config.sh',
      f'--with-system-gdbinit=/share/gdb/gdbinit',
      *cflags,
    ])

    # workaround: top level configure does not pass CPPFLAGS to gdbsupport.
    # thus it defaults to _WIN32_WINNT=0x0501, breaking win32 and mcf thread model.
    for item in cflags:
      if item.startswith('CPPFLAGS='):
        os.environ['CPPFLAGS'] = item[len('CPPFLAGS='):]
    make_custom(build_dir, ['configure-host'], config.jobs)
    del os.environ['CPPFLAGS']

    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_ABB.gdb)

    gdbinit = paths.layer_ABB.gdb / 'share/gdb/gdbinit'
    with open(gdbinit, 'w') as f:
      pass

    shutil.copy(f'/usr/local/{ver.target}/lib/python.zip', paths.layer_ABB.gdb / 'lib/python.zip')
    with open(paths.layer_ABB.gdb / 'bin/gdb._pth', 'w') as f:
      f.write('../lib/python.zip\n')
    with open(gdbinit, 'a') as f:
      f.write('python\n')
      f.write('from libstdcxx.v6.printers import register_libstdcxx_printers\n')
      f.write('register_libstdcxx_printers(None)\n')
      f.write('end\n')

  remove_info_main_menu(paths.layer_ABB.gdb)

  # collision with binutils
  for info_file in ['bfd.info', 'ctf-spec.info', 'sframe-spec.info']:
    os.unlink(paths.layer_ABB.gdb / 'share/info' / info_file)

  license_dir = paths.layer_ABB.gdb / 'share/licenses/gdb'
  ensure(license_dir)
  for file in ['COPYING', 'COPYING3', 'COPYING.LIB', 'COPYING3.LIB']:
    shutil.copy(paths.src_dir.gdb / file, license_dir / file)

def _gmake(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    # override CRT
    paths.layer_AAB.utf8 / 'usr/local',

    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),

    paths.layer_AAB.iconv / 'usr/local',
    paths.layer_AAB.intl / 'usr/local',
  ]):
    v = Version(ver.make)
    v_gcc = Version(ver.gcc)
    build_dir = paths.src_dir.make / 'build-ABB'
    ensure(build_dir)

    c_extra = []

    # GCC 15 defaults to C23, in which `foo()` means `foo(void)` instead of `foo(...)`.
    if v_gcc.major >= 15 and v < Version('4.5'):
      c_extra.append('-std=gnu11')

    configure(build_dir, [
      '--prefix=',
      '--program-prefix=mingw32-',
      f'--host={ver.target}',
      f'--build={config.build}',
      '--enable-nls',
      *cflags_B(
        cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
        c_extra = c_extra,
        optimize_for_speed = ver.opt_speed,
      ),
    ])
    make_default(build_dir, config.jobs)
    make_destdir_install(build_dir, paths.layer_ABB.make)

  remove_info_main_menu(paths.layer_ABB.make)

  license_dir = paths.layer_ABB.make / 'share/licenses/make'
  ensure(license_dir)
  shutil.copy(paths.src_dir.make / 'COPYING', license_dir / 'COPYING')

def _pkgconf(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.python / 'usr/local',
    paths.layer_AAA.meson / 'usr/local',

    # override CRT
    paths.layer_AAB.utf8 / 'usr/local',

    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),
  ]):
    build_dir = 'build-ABB'

    meson_config(
      paths.src_dir.pkgconf,
      extra_args = [
        '--cross-file', paths.meson_cross_file,
        '--prefix', '/',
        '--default-library', 'static',
        '--prefer-static',
        '-Dtests=disabled',
        *meson_flags_B(
          cpp_extra = [f'-D_WIN32_WINNT=0x{ver.min_winnt:04X}'],
          optimize_for_speed = ver.opt_speed,
        ),
      ],
      build_dir = build_dir,
    )
    meson_build(
      paths.src_dir.pkgconf,
      jobs = config.jobs,
      build_dir = build_dir,
    )
    meson_install(
      paths.src_dir.pkgconf,
      build_dir = build_dir,
      destdir = paths.layer_ABB.pkgconf,
    )

  license_dir = paths.layer_ABB.pkgconf / 'share/licenses/pkgconf'
  ensure(license_dir)
  shutil.copy(paths.src_dir.pkgconf / 'COPYING', license_dir / 'COPYING')

def build_ABB_toolchain(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _binutils(ver, paths, config)
  _headers(ver, paths, config)
  _crt0(ver, paths, config)
  _crt(ver, paths, config)
  _winpthreads(ver, paths, config)
  _mcfgthread(ver, paths, config)
  _nowide(ver, paths, config)
  _gcc_1(ver, paths, config)
  _gcc_2(ver, paths, config)
  _gdb(ver, paths, config)
  _gmake(ver, paths, config)
  _pkgconf(ver, paths, config)

def _xmake(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  with overlayfs_ro('/usr/local', [
    paths.layer_AAA.xmake / 'usr/local',

    # override CRT
    paths.layer_AAB.utf8 / 'usr/local',

    paths.layer_AAB.crt_host / 'usr/local',
    *common_cross_layers(paths),
  ]):
    build_dir = paths.src_dir.xmake / 'core'
    xmake_config(build_dir, [
      '-m', 'releasedbg',
      '--plat=mingw',
      f'--arch={XMAKE_ARCH_MAP[ver.arch]}',
      '--mingw=/usr/local',
    ])
    xmake_build(build_dir, config.jobs)
    xmake_install(build_dir, paths.layer_ABB.xmake, ['cli'])

  license_dir = paths.layer_ABB.xmake / 'share/licenses/xmake'
  ensure(license_dir)
  shutil.copy(paths.src_dir.xmake / 'LICENSE.md', license_dir / 'LICENSE.md')

def build_ABB_xmake(ver: BranchProfile, paths: ProjectPaths, config: argparse.Namespace):
  _xmake(ver, paths, config)
