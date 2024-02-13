#!/bin/bash

set -euxo pipefail

cd "$_ASSETS_DIR"
[[ -f "$_BINUTILS_ARCHIVE" ]] || curl -LO "http://ftp.gnu.org/gnu/binutils/$_BINUTILS_ARCHIVE"
[[ -f "$_MINGW_ARCHIVE" ]] || curl -LO "https://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/$_MINGW_ARCHIVE"
[[ -f "$_GCC_ARCHIVE" ]] || curl -LO "http://ftp.gnu.org/gnu/gcc/gcc-$GCC_VER/$_GCC_ARCHIVE"
[[ -f "$_GMP_ARCHIVE" ]] || curl -LO "https://ftp.gnu.org/gnu/gmp/$_GMP_ARCHIVE"
[[ -f "$_MPFR_ARCHIVE" ]] || curl -LO "https://ftp.gnu.org/gnu/mpfr/$_MPFR_ARCHIVE"
[[ -f "$_MPC_ARCHIVE" ]] || curl -LO "https://ftp.gnu.org/gnu/mpc/$_MPC_ARCHIVE"
[[ -f "$_ICONV_ARCHIVE" ]] || curl -LO "https://ftp.gnu.org/gnu/libiconv/$_ICONV_ARCHIVE"
[[ -f "$_GDB_ARCHIVE" ]] || curl -LO "http://ftp.gnu.org/gnu/gdb/$_GDB_ARCHIVE"
[[ -f "$_MAKE_ARCHIVE" ]] || curl -LO "http://ftp.gnu.org/gnu/make/$_MAKE_ARCHIVE"

cd "$_BUILD_DIR"
[[ -d "$_BINUTILS_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_BINUTILS_ARCHIVE" --no-same-owner
[[ -d "$_MINGW_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_MINGW_ARCHIVE" --no-same-owner
[[ -d "$_GCC_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_GCC_ARCHIVE" --no-same-owner
[[ -d "$_GMP_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_GMP_ARCHIVE" --no-same-owner
[[ -d "$_MPFR_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_MPFR_ARCHIVE" --no-same-owner
[[ -d "$_MPC_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_MPC_ARCHIVE" --no-same-owner
[[ -d "$_ICONV_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_ICONV_ARCHIVE" --no-same-owner
[[ -d "$_GDB_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_GDB_ARCHIVE" --no-same-owner
[[ -d "$_MAKE_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_MAKE_ARCHIVE" --no-same-owner

case "$_WIN32_WINNT" in
  0x0501|0x0502)
    # skeeto/w64devkit#58: disable UTF-8 manifest for XP
    echo >"$_GCC_DIR/gcc/config/i386/winnt-utf8.manifest"
    ;;
esac
