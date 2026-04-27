from hashlib import sha256
import logging
import os
from packaging.version import Version
from pathlib import Path
import re
import shutil
import subprocess
from urllib.error import URLError
from urllib.request import urlopen

from module.fetch import validate_and_download, check_and_extract, check_and_sync, patch, patch_done
from module.path import ProjectPaths
from module.profile import BranchProfile

def _atomic_bootstrap(ver: BranchProfile, paths: ProjectPaths):
  shutil.copytree(
    paths.in_tree_src_tree.atomic_bootstrap,
    paths.in_tree_src_dir.atomic_bootstrap,
    ignore = shutil.ignore_patterns(
      '.cache',
      '.vscode',
      '.xmake',
      'build',
    ),
    dirs_exist_ok = True,
  )

def _binutils(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://ftpmirror.gnu.org/gnu/binutils/{paths.src_arx.binutils.name}'
  validate_and_download(paths.src_arx.binutils, url)
  if download_only:
    return

  if check_and_extract(paths.src_dir.binutils, paths.src_arx.binutils):
    v = Version(ver.binutils)

    # Fix development flag
    if v == Version('2.46.0'):
      patch(paths.src_dir.binutils, paths.patch_dir / 'binutils' / 'fix-development-flag.patch')

    # Fix path corruption
    if v >= Version('2.46'):
      patch(paths.src_dir.binutils, paths.patch_dir / 'binutils' / 'fix-path-corruption_2.46.patch')
    elif v >= Version('2.43'):
      patch(paths.src_dir.binutils, paths.patch_dir / 'binutils' / 'fix-path-corruption_2.43.patch')
    else:
      # early implementation is so buggy that we simply ignore it
      # see below "Ingore long path"
      pass

    # Don't optimize out libtool wrapper magic
    patch(paths.src_dir.binutils, paths.patch_dir / 'binutils' / 'dont-optimize-out-libtool-wrapper-magic.patch')

    # Ignore long path
    if v >= Version('2.43'):
      if ver.min_os.major < 4:
        patch(paths.src_dir.binutils, paths.patch_dir / 'binutils' / 'ignore-long-path_9x.patch')
    else:
      patch(paths.src_dir.binutils, paths.patch_dir / 'binutils' / 'ignore-long-path_buggy.patch')

    patch_done(paths.src_dir.binutils)

def _expat(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  v = Version(ver.expat)
  tag = f'R_{v.major}_{v.minor}_{v.micro}'
  url = f'https://github.com/libexpat/libexpat/releases/download/{tag}/{paths.src_arx.expat.name}'
  validate_and_download(paths.src_arx.expat, url)
  if download_only:
    return

  check_and_extract(paths.src_dir.expat, paths.src_arx.expat)
  patch_done(paths.src_dir.expat)

def _gcc(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  v = Version(ver.gcc)

  is_snapshot = re.search(r'-\d{8}$', ver.gcc)
  if is_snapshot:
    url = f'https://gcc.gnu.org/pub/gcc/snapshots/{ver.gcc}/{paths.src_arx.gcc.name}'
  else:
    url = f'https://ftpmirror.gnu.org/gnu/gcc/gcc-{ver.gcc}/{paths.src_arx.gcc.name}'

  validate_and_download(paths.src_arx.gcc, url)
  if download_only:
    return

  if check_and_extract(paths.src_dir.gcc, paths.src_arx.gcc):
    # Disable default utf8 manifest
    # We patch the CRT init objects, so GCC's manifest should be disabled.
    # But we want no `--disable-win32-utf8-manifest` in configure flags to avoid confusion.
    if not ver.utf8_thunk:
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'disable-default-utf8-manifest.patch')

    if ver.utf8_user_crt:
      # U8CRT: spec
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'u8crt-spec-mcrtdll.patch')
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'u8crt-spec-mutf8.patch')
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'u8crt-spec-stdlib.patch')
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'u8crt-build-libutf8-stdc++.patch')

      # U8CRT: validate UTF-8
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'u8crt-validate-utf8.patch')

    # Use Linux style tooldir
    # MinGW Lite install binutils with `tooldir=$prefix`, the common practice in Linux.
    # However, GCC tries to locate the tools in $prefix/$triplet and then falls back to PATH.
    # It will fail without PATH, or even worse, calls unexpected tools from other toolchain.
    # We adjust the strategy to make it work without PATH.
    if v.major >= 15:
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'use-linux-style-tooldir_15.patch')
    else:
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'use-linux-style-tooldir_13.patch')

    # Use mingw32-make for LTO
    # parallel LTO may call incompatible, POSIX `make.exe`
    # https://github.com/redpanda-cpp/mingw-lite/issues/12
    patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'use-mingw32-make-for-lto.patch')

    # Fix make variable
    # - gcc 12 use `override CFLAGS +=` to handle PGO build, which breaks workaround for ucrt `access`
    if v.major >= 14:
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-make-variable_14.patch')
    else:
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-make-variable_12.patch')

    # Allow missing shared libgcc
    patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'allow-missing-shared-libgcc.patch')

    # Fix pre-586 atomic link
    if ver.march in ('i386', 'i486'):
      if v.major >= 14:
        patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-pre586-atomic-link_14.patch')
      else:
        patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-pre586-atomic-link_13.patch')

    # Migrate i386 sync builtin
    if ver.march == 'i386':
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'migrate-i386-sync-builtin.patch')

    # Allow UTF-8 module name
    patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'allow-utf8-module-name.patch')

    # Fix preprocessor encoding
    # - __FILE__ macro encoding
    # - #include directive encoding
    if not ver.utf8_thunk:
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-pp-encoding.patch')

    # Fix VT sequence
    patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-vt-seq.patch')

    # Fix libcpp setlocale
    # libcpp defines `setlocale` if `HAVE_SETLOCALE` not defined, but its configure.ac does not check `setlocale` at all
    patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-libcpp-setlocale.patch')

    # Fix ostream::operator<< call frame
    # The function
    #   ostream &ostream::operator<<(ostream &(*func)(ostream &))
    #   {
    #     return func(*this);
    #   }
    # is a tail call, and can be optimized to
    #   jmp *rdx
    # But the missing call frame can fool the debugger, breaking its "step over" function.
    if v.major >= 15:
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-ostream-left-shift-operator-call-frame_15.patch')
    else:
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-ostream-left-shift-operator-call-frame_13.patch')

    # Dynamically load tzdb
    if v.major >= 16:
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'dynamic-load-tzdb.patch')

    # Fix NUMA unaware build
    if v.major >= 16:
      patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'fix-numa-unaware-build.patch')

    # Disable vectorized lexer
    if ver.min_os.major < 5:
      if v.major >= 15:
        patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'disable-vectorized-lexer_15.patch')
      else:
        patch(paths.src_dir.gcc, paths.patch_dir / 'gcc' / 'disable-vectorized-lexer_13.patch')

    # Parser-friendly diagnostics
    po_dir = paths.src_dir.gcc / 'gcc' / 'po'
    po_files = list(po_dir.glob('*.po'))
    subprocess.run([
      'sed',
      '-i', '-E',
      '/^msgid "(error|warning): "/,+1 d',
      *po_files
    ], check = True)

    patch_done(paths.src_dir.gcc)

def _gcc_lib_bootstrap(ver: BranchProfile, paths: ProjectPaths):
  shutil.copytree(
    paths.in_tree_src_tree.gcc_lib_bootstrap,
    paths.in_tree_src_dir.gcc_lib_bootstrap,
    ignore = shutil.ignore_patterns(
      '.cache',
      '.vscode',
      '.xmake',
      'build',
    ),
    dirs_exist_ok = True,
  )

def _gdb(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://ftpmirror.gnu.org/gnu/gdb/{paths.src_arx.gdb.name}'
  validate_and_download(paths.src_arx.gdb, url)
  if download_only:
    return

  if check_and_extract(paths.src_dir.gdb, paths.src_arx.gdb):
    v = Version(ver.gdb)

    # Disable runtime linking -A/-W
    if ver.utf8_thunk:
      patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'disable-runtime-linking-aw.patch')

    # Fix path corruption
    if v.major >= 15:
      patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-path-corruption_15.patch')
    else:
      # early implementation is so buggy that we simply ignore it
      # see below "Ingore long path"
      pass

    # Fix thread
    if v.major >= 16:
      patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-thread_16.patch')
    else:
      patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-thread_14.patch')

    # Fix iconv 'CP65001'
    patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-iconv-cp65001.patch')

    # Fix pythondir
    if not ver.utf8_thunk:
      patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-pythondir.patch')

    # Fix 'skip gfile' `fnmatch`
    if v.major == 17:
      patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-skip-gfile-fnmatch.patch')

    # Ignore long path
    if v.major >= 15:
      if ver.min_os.major < 4:
        patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'ignore-long-path_9x.patch')
    else:
      patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'ignore-long-path_buggy.patch')

    if ver.min_os.major < 4:
      # Fix 9x select
      patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-9x-select.patch')

      # Fix 9x negative P/TID
      patch(paths.src_dir.gdb, paths.patch_dir / 'gdb' / 'fix-9x-negative-ptid.patch')

    patch_done(paths.src_dir.gdb)

def _gmp(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://ftpmirror.gnu.org/gnu/gmp/{paths.src_arx.gmp.name}'
  validate_and_download(paths.src_arx.gmp, url)
  if download_only:
    return

  check_and_extract(paths.src_dir.gmp, paths.src_arx.gmp)
  patch_done(paths.src_dir.gmp)

def _iconv_gnu(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://ftpmirror.gnu.org/gnu/libiconv/{paths.src_arx.iconv.name}'
  validate_and_download(paths.src_arx.iconv, url)
  if download_only:
    return

  if check_and_extract(paths.src_dir.iconv, paths.src_arx.iconv):
    # Fix rc naming convention
    patch(paths.src_dir.iconv, paths.patch_dir / 'iconv' / 'fix-rc-naming-convention.patch')

    patch_done(paths.src_dir.iconv)

def _iconv_win32(ver: BranchProfile, paths: ProjectPaths):
  shutil.copytree(
    paths.in_tree_src_tree.iconv,
    paths.in_tree_src_dir.iconv,
    ignore = shutil.ignore_patterns(
      '.cache',
      '.vscode',
      '.xmake',
      'build',
      'pkg',
    ),
    dirs_exist_ok = True,
  )

def _intl(ver: BranchProfile, paths: ProjectPaths):
  shutil.copytree(
    paths.in_tree_src_tree.intl,
    paths.in_tree_src_dir.intl,
    ignore = shutil.ignore_patterns(
      '.cache',
      '.vscode',
      '.xmake',
      'build',
      'pkg',
      '*.mo',
    ),
    dirs_exist_ok = True,
  )

def _make(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://ftpmirror.gnu.org/gnu/make/{paths.src_arx.make.name}'
  validate_and_download(paths.src_arx.make, url)
  if download_only:
    return

  check_and_extract(paths.src_dir.make, paths.src_arx.make)
  patch_done(paths.src_dir.make)

def _mcfgthread(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://github.com/lhmouse/mcfgthread/archive/refs/tags/v{ver.mcfgthread}.tar.gz'
  validate_and_download(paths.src_arx.mcfgthread, url)
  if download_only:
    return

  if check_and_extract(paths.src_dir.mcfgthread, paths.src_arx.mcfgthread):
    v = Version(ver.mcfgthread.replace('-ga', ''))

    # Fix bootstrap
    if v < Version('2.1'):
      patch(paths.src_dir.mcfgthread, paths.patch_dir / 'mcfgthread' / 'fix-bootstrap.patch')

    patch_done(paths.src_dir.mcfgthread)

def _meson(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://github.com/mesonbuild/meson/releases/download/{ver.meson}/{paths.src_arx.meson.name}'
  validate_and_download(paths.src_arx.meson, url)
  if download_only:
    return

  check_and_extract(paths.src_dir.meson, paths.src_arx.meson)
  patch_done(paths.src_dir.meson)

def _mingw(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/{paths.src_arx.mingw.name}'
  validate_and_download(paths.src_arx.mingw, url)
  if download_only:
    return

  if check_and_extract(paths.src_dir.mingw, paths.src_arx.mingw):
    v = Version(ver.mingw)

    # CRT: Fix import module name
    if v.major >= 12:
      patch(paths.src_dir.mingw, paths.patch_dir / 'crt' / 'fix-import-module-name_12.patch')
    else:
      patch(paths.src_dir.mingw, paths.patch_dir / 'crt' / 'fix-import-module-name_11.patch')

    # CRT: Fix emu
    if v.major >= 13:
      pass
    elif v.major >= 12:
      patch(paths.src_dir.mingw, paths.patch_dir / 'crt' / 'fix-emu_12.patch')
    else:
      patch(paths.src_dir.mingw, paths.patch_dir / 'crt' / 'fix-emu_11.patch')

    # CRT and winpthreads: migrate i386 sync builtin
    if ver.march == 'i386':
      if v.major >= 13:
        patch(paths.src_dir.mingw, paths.patch_dir / 'crt' / 'migrate-i386-sync-builtin_13.patch')
      else:
        patch(paths.src_dir.mingw, paths.patch_dir / 'crt' / 'migrate-i386-sync-builtin_11.patch')

      if v.major < 12:
        patch(paths.src_dir.mingw, paths.patch_dir / 'winpthreads' / 'migrate-i386-sync-builtin.patch')

    # Winpthreads: Fix linkage
    if v.major == 13:
      patch(paths.src_dir.mingw, paths.patch_dir / 'winpthreads' / 'fix-linkage.patch')

    patch_done(paths.src_dir.mingw)

def _mpc(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://ftpmirror.gnu.org/gnu/mpc/{paths.src_arx.mpc.name}'
  validate_and_download(paths.src_arx.mpc, url)
  if download_only:
    return

  check_and_extract(paths.src_dir.mpc, paths.src_arx.mpc)
  patch_done(paths.src_dir.mpc)

def _mpfr(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://ftpmirror.gnu.org/gnu/mpfr/{paths.src_arx.mpfr.name}'
  validate_and_download(paths.src_arx.mpfr, url)
  if download_only:
    return

  check_and_extract(paths.src_dir.mpfr, paths.src_arx.mpfr)
  patch_done(paths.src_dir.mpfr)

def _nowide(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://github.com/boostorg/nowide/releases/download/v{ver.nowide}/{paths.src_arx.nowide.name}'
  validate_and_download(paths.src_arx.nowide, url)
  if download_only:
    return

  check_and_extract(paths.src_dir.nowide, paths.src_arx.nowide)
  patch_done(paths.src_dir.nowide)

def _pdcurses(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://github.com/wmcbrine/PDCurses/archive/refs/tags/{ver.pdcurses}.tar.gz'
  validate_and_download(paths.src_arx.pdcurses, url)
  if download_only:
    return

  check_and_extract(paths.src_dir.pdcurses, paths.src_arx.pdcurses)
  patch_done(paths.src_dir.pdcurses)

def _pkgconf(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://github.com/pkgconf/pkgconf/archive/refs/tags/pkgconf-{ver.pkgconf}.tar.gz'
  validate_and_download(paths.src_arx.pkgconf, url)
  if download_only:
    return

  if check_and_extract(paths.src_dir.pkgconf, paths.src_arx.pkgconf):
    ver = Version(ver.pkgconf)

    # Build for static toolchain
    if ver >= Version('2.5.0'):
      patch(paths.src_dir.pkgconf, paths.patch_dir / 'pkgconf/static-toolchain_2.5.patch')
    else:
      patch(paths.src_dir.pkgconf, paths.patch_dir / 'pkgconf/static-toolchain_2.1.patch')

    patch_done(paths.src_dir.pkgconf)

def _python(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://www.python.org/ftp/python/{ver.python}/{paths.src_arx.python.name}'
  validate_and_download(paths.src_arx.python, url)
  if download_only:
    return

  if check_and_extract(paths.src_dir.python, paths.src_arx.python):
    ver = Version(ver.python)

    # Fix static build
    if ver >= Version('3.14'):
      patch(paths.src_dir.python, paths.patch_dir / 'python' / 'fix-static-build.patch')

    # Fix thread touch last error
    # https://github.com/python/cpython/pull/104531
    if ver >= Version('3.12') and ver < Version('3.13'):
      patch(paths.src_dir.python, paths.patch_dir / 'python' / 'fix-thread-touch-last-error_3.12.patch')

    # Alternative build system
    os.symlink(paths.src_dir.z, paths.src_dir.python / 'zlib', target_is_directory = True)
    if ver >= Version('3.14'):
      shutil.copy(paths.patch_dir / 'python' / 'xmake_3.14.lua', paths.src_dir.python / 'xmake.lua')
      patch(paths.src_dir.python, paths.patch_dir / 'python' / 'fix-mingw-build_3.14.patch')
    elif ver >= Version('3.13'):
      shutil.copy(paths.patch_dir / 'python' / 'xmake_3.13.lua', paths.src_dir.python / 'xmake.lua')
      patch(paths.src_dir.python, paths.patch_dir / 'python' / 'fix-mingw-build_3.13.patch')
    else:
      shutil.copy(paths.patch_dir / 'python' / 'xmake_3.12.lua', paths.src_dir.python / 'xmake.lua')
      patch(paths.src_dir.python, paths.patch_dir / 'python' / 'fix-mingw-build_3.12.patch')
    shutil.copy(paths.patch_dir / 'python' / 'python-config.sh', paths.src_dir.python / 'python-config.sh')

    patch_done(paths.src_dir.python)

def _setuptools(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://github.com/pypa/setuptools/archive/refs/tags/v{ver.setuptools}.tar.gz'
  validate_and_download(paths.src_arx.setuptools, url)
  if download_only:
    return

  check_and_extract(paths.src_dir.setuptools, paths.src_arx.setuptools)
  patch_done(paths.src_dir.setuptools)

def _thunk(ver: BranchProfile, paths: ProjectPaths):
  shutil.copytree(
    paths.in_tree_src_tree.thunk,
    paths.in_tree_src_dir.thunk,
    ignore = shutil.ignore_patterns(
      '.cache',
      '.vscode',
      '.xmake',
      'build',
    ),
    dirs_exist_ok = True,
  )

def _xmake(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  release_name = paths.src_arx.xmake.name.replace('xmake-', 'xmake-v')
  url = f'https://github.com/xmake-io/xmake/releases/download/v{ver.xmake}/{release_name}'
  validate_and_download(paths.src_arx.xmake, url)
  if download_only:
    return

  if check_and_extract(paths.src_dir.xmake, paths.src_arx.xmake):
    tbox = paths.src_dir.xmake / 'core/src/tbox/tbox'

    # disable werror
    xmake_lua = paths.src_dir.xmake / 'core/xmake.lua'
    with open(xmake_lua, 'r') as f:
      xmake_lua_content = f.readlines()
    with open(xmake_lua, 'w') as f:
      for line in xmake_lua_content:
        if line.startswith('set_warnings'):
          f.write('set_warnings("all")\n')
        else:
          f.write(line)

    # DOS capital path
    patch(paths.src_dir.xmake, paths.patch_dir / 'xmake/DOS-CAPITAL-PATH.patch')

    # Fix MinGW link
    patch(paths.src_dir.xmake, paths.patch_dir / 'xmake/fix-mingw-link.patch')

    # Tbox: ignore process group
    patch(tbox, paths.patch_dir / 'xmake/tbox-ignore-process-group.patch')

    # Tbox: reduce runtime linking
    patch(tbox, paths.patch_dir / 'xmake/tbox-reduce-runtime-linking.patch')

    # Tbox: fix 9x file time range
    patch(tbox, paths.patch_dir / 'xmake/tbox-fix-9x-file-time-range.patch')

    # Tbox: 9x IOCP fallback
    patch(tbox, paths.patch_dir / 'xmake/tbox-9x-iocp-fallback.patch')

    patch_done(paths.src_dir.xmake)

def _z(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  url = f'https://github.com/madler/zlib/releases/download/v{ver.z}/{paths.src_arx.z.name}'
  validate_and_download(paths.src_arx.z, url)
  if download_only:
    return

  check_and_extract(paths.src_dir.z, paths.src_arx.z)
  patch_done(paths.src_dir.z)

def prepare_source(ver: BranchProfile, paths: ProjectPaths, download_only: bool):
  _atomic_bootstrap(ver, paths)
  _binutils(ver, paths, download_only)
  _expat(ver, paths, download_only)
  _gcc(ver, paths, download_only)
  _gcc_lib_bootstrap(ver, paths)
  _gdb(ver, paths, download_only)
  _gmp(ver, paths, download_only)
  if ver.iconv_win32:
    if not download_only:
      _iconv_win32(ver, paths)
  else:
    _iconv_gnu(ver, paths, download_only)
  _intl(ver, paths)
  _make(ver, paths, download_only)
  _mcfgthread(ver, paths, download_only)
  _meson(ver, paths, download_only)
  _mingw(ver, paths, download_only)
  _mpc(ver, paths, download_only)
  _mpfr(ver, paths, download_only)
  _nowide(ver, paths, download_only)
  _pdcurses(ver, paths, download_only)
  _pkgconf(ver, paths, download_only)
  _python(ver, paths, download_only)
  _setuptools(ver, paths, download_only)
  _thunk(ver, paths)
  _xmake(ver, paths, download_only)
  _z(ver, paths, download_only)
