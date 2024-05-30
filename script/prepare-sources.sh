#!/bin/bash

set -euxo pipefail

cd "$_ASSETS_DIR"
[[ -f "$_BINUTILS_ARCHIVE" ]] || curl -LO "http://ftp.gnu.org/gnu/binutils/$_BINUTILS_ARCHIVE"
[[ -f "$_MINGW_ARCHIVE" ]] || curl -LO "https://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/$_MINGW_ARCHIVE"
if [[ "$_THREAD" == "mcf" ]]; then
  [[ -f "$_MCFGTHREAD_ARCHIVE" ]] || curl -L -o "$_MCFGTHREAD_ARCHIVE" "https://github.com/lhmouse/mcfgthread/archive/refs/tags/v$_MCFGTHREAD_VER.tar.gz"
fi
[[ -f "$_GCC_ARCHIVE" ]] || curl -LO "http://ftp.gnu.org/gnu/gcc/gcc-$_GCC_VER/$_GCC_ARCHIVE"
[[ -f "$_GMP_ARCHIVE" ]] || curl -LO "https://ftp.gnu.org/gnu/gmp/$_GMP_ARCHIVE"
[[ -f "$_MPFR_ARCHIVE" ]] || curl -LO "https://ftp.gnu.org/gnu/mpfr/$_MPFR_ARCHIVE"
[[ -f "$_MPC_ARCHIVE" ]] || curl -LO "https://ftp.gnu.org/gnu/mpc/$_MPC_ARCHIVE"
[[ -f "$_ICONV_ARCHIVE" ]] || curl -LO "https://ftp.gnu.org/gnu/libiconv/$_ICONV_ARCHIVE"
[[ -f "$_GETTEXT_ARCHIVE" ]] || curl -LO "https://ftp.gnu.org/gnu/gettext/$_GETTEXT_ARCHIVE"
[[ -f "$_GDB_ARCHIVE" ]] || curl -LO "http://ftp.gnu.org/gnu/gdb/$_GDB_ARCHIVE"
[[ -f "$_MAKE_ARCHIVE" ]] || curl -LO "http://ftp.gnu.org/gnu/make/$_MAKE_ARCHIVE"

cd "$_BUILD_DIR"
if [[ ! -d "$_BINUTILS_DIR" ]]; then
  bsdtar -xf "$_ASSETS_DIR/$_BINUTILS_ARCHIVE" --no-same-owner
  patch -d "$_BINUTILS_DIR" -Np1 <"$_PATCH_DIR/binutils-fix-path-corruption.patch"
fi
if [[ ! -d "$_MINGW_DIR" ]]; then
  bsdtar -xf "$_ASSETS_DIR/$_MINGW_ARCHIVE" --no-same-owner
  patch -d "$_MINGW_DIR" -Np1 <"$_PATCH_DIR/crt-fix-missing-function.patch"
  [[ $_WINPTHREADS_USE_VEH -eq 0 ]] && patch -d "$_MINGW_DIR" -Np1 <"$_PATCH_DIR/winpthreads-disable-veh.patch"
fi
if [[ "$_THREAD" == "mcf" && ! -d "$_MCFGTHREAD_DIR" ]]; then
  bsdtar -xf "$_ASSETS_DIR/$_MCFGTHREAD_ARCHIVE" --no-same-owner
  ln -s meson.options "$_MCFGTHREAD_DIR/meson_options.txt"
fi
if [[ ! -d "$_GCC_DIR" ]]; then
  bsdtar -xf "$_ASSETS_DIR/$_GCC_ARCHIVE" --no-same-owner
  patch -d "$_GCC_DIR" -Np1 <"$_PATCH_DIR/gcc-fix-console-cp.patch"
  patch -d "$_GCC_DIR" -Np1 <"$_PATCH_DIR/gcc-fix-localedir.patch"
  [[ $_UTF8_MANIFEST -eq 0 ]] && echo >"$_GCC_DIR/gcc/config/i386/winnt-utf8.manifest"
  [[ $_GCCLIB_USE_ALIGNED_MALLOC -eq 0 ]] && patch -d "$_GCC_DIR" -Np1 <"$_PATCH_DIR/gcclib-disable-aligned-malloc.patch"

  # do not translate "error: " and "warning: "
  sed -i -E '/^msgid "(error|warning): "/,+1 d' "$_GCC_DIR"/gcc/po/*.po
fi
[[ -d "$_GMP_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_GMP_ARCHIVE" --no-same-owner
[[ -d "$_MPFR_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_MPFR_ARCHIVE" --no-same-owner
[[ -d "$_MPC_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_MPC_ARCHIVE" --no-same-owner
[[ -d "$_ICONV_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_ICONV_ARCHIVE" --no-same-owner
[[ -d "$_GETTEXT_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_GETTEXT_ARCHIVE" --no-same-owner
if [[ ! -d "$_GDB_DIR" ]]; then
  bsdtar -xf "$_ASSETS_DIR/$_GDB_ARCHIVE" --no-same-owner
  [[ "$_WIN32_WINNT" -lt 0x0601 ]] && patch -d "$_GDB_DIR" -Np1 <"$_PATCH_DIR/gdb-fix-xp-vista-x64.patch"
  if [[ "$_WIN32_WINNT" -lt 0x0501 ]]; then
    cp "$_PATCH_DIR/gdb-nt50-win32thunk.h" "$_GDB_DIR/gdb/win32thunk.h"
    patch -d "$_GDB_DIR" -Np1 <"$_PATCH_DIR/gdb-nt50-win32thunk.patch"
  fi
fi
[[ -d "$_MAKE_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_MAKE_ARCHIVE" --no-same-owner
