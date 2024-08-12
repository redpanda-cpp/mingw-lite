#!/bin/bash

set -euxo pipefail

function fn-version-ge() {
  printf '%s\n%s\n' "$2" "$1" | sort --check=quiet --version-sort
}

############
# Download #
############

cd "$_ASSETS_DIR"
[[ -f "$_BINUTILS_ARCHIVE" ]] || curl -LO "http://ftpmirror.gnu.org/gnu/binutils/$_BINUTILS_ARCHIVE"
[[ -f "$_MINGW_ARCHIVE" ]] || curl -LO "https://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/$_MINGW_ARCHIVE"
if [[ "$_THREAD" == "mcf" ]]; then
  [[ -f "$_MCFGTHREAD_ARCHIVE" ]] || curl -L -o "$_MCFGTHREAD_ARCHIVE" "https://github.com/lhmouse/mcfgthread/archive/refs/tags/v$_MCFGTHREAD_VER.tar.gz"
fi
if [[ ! -f "$_GCC_ARCHIVE" ]]; then
  if fn-version-ge "$_GCC_VER" "15"; then
    curl -LO "https://gcc.gnu.org/pub/gcc/snapshots/$_GCC_VER/$_GCC_ARCHIVE"
  else
    curl -LO "http://ftpmirror.gnu.org/gnu/gcc/gcc-$_GCC_VER/$_GCC_ARCHIVE"
  fi
fi
[[ -f "$_GMP_ARCHIVE" ]] || curl -LO "https://ftpmirror.gnu.org/gnu/gmp/$_GMP_ARCHIVE"
[[ -f "$_MPFR_ARCHIVE" ]] || curl -LO "https://ftpmirror.gnu.org/gnu/mpfr/$_MPFR_ARCHIVE"
[[ -f "$_MPC_ARCHIVE" ]] || curl -LO "https://ftpmirror.gnu.org/gnu/mpc/$_MPC_ARCHIVE"
[[ -f "$_ICONV_ARCHIVE" ]] || curl -LO "https://ftpmirror.gnu.org/gnu/libiconv/$_ICONV_ARCHIVE"
[[ -f "$_GETTEXT_ARCHIVE" ]] || curl -LO "https://ftpmirror.gnu.org/gnu/gettext/$_GETTEXT_ARCHIVE"
[[ -f "$_GDB_ARCHIVE" ]] || curl -LO "http://ftpmirror.gnu.org/gnu/gdb/$_GDB_ARCHIVE"
[[ -f "$_MAKE_ARCHIVE" ]] || curl -LO "http://ftpmirror.gnu.org/gnu/make/$_MAKE_ARCHIVE"

#####################
# Extract and Patch #
#####################

cd "$_BUILD_DIR"

## Binutils
if [[ ! -d "$_BINUTILS_DIR" ]]; then
  bsdtar -xf "$_ASSETS_DIR/$_BINUTILS_ARCHIVE" --no-same-owner

  # Fix path corruption
  if fn-version-ge $_BINUTILS_VER 2.43; then
    patch -d "$_BINUTILS_DIR" -Np1 <"$_PATCH_DIR/binutils/2.43-fix-path-corruption.patch"
  else
    patch -d "$_BINUTILS_DIR" -Np1 <"$_PATCH_DIR/binutils/2.42-fix-path-corruption.patch"
  fi
fi

## MinGW-w64
if [[ ! -d "$_MINGW_DIR" ]]; then
  bsdtar -xf "$_ASSETS_DIR/$_MINGW_ARCHIVE" --no-same-owner

  # CRT: Fix missing function
  [[ $_WIN32_WINNT -lt 0x0600 ]] && patch -d "$_MINGW_DIR" -Np1 <"$_PATCH_DIR/crt/fix-missing-function.patch"

  # winpthreads: Disable VEH
  [[ $_WIN32_WINNT -lt 0x0501 ]] && patch -d "$_MINGW_DIR" -Np1 <"$_PATCH_DIR/winpthreads/disable-veh.patch"
fi

## MCF Gthread
if [[ "$_THREAD" == "mcf" && ! -d "$_MCFGTHREAD_DIR" ]]; then
  bsdtar -xf "$_ASSETS_DIR/$_MCFGTHREAD_ARCHIVE" --no-same-owner
fi

## GCC
if [[ ! -d "$_GCC_DIR" ]]; then
  bsdtar -xf "$_ASSETS_DIR/$_GCC_ARCHIVE" --no-same-owner

  # Fix VT sequence
  patch -d "$_GCC_DIR" -Np1 <"$_PATCH_DIR/gcc/fix-vt-seq.patch"

  # Fix locale directory
  patch -d "$_GCC_DIR" -Np1 <"$_PATCH_DIR/gcc/fix-localedir.patch"

  # Parser-friendly diagnostics
  sed -i -E '/^msgid "(error|warning): "/,+1 d' "$_GCC_DIR"/gcc/po/*.po

  if [[ $_HOST_WINNT -ge 0x0600 ]]; then
    # Fix console code page
    patch -d "$_GCC_DIR" -Np1 <"$_PATCH_DIR/gcc/fix-console-cp.patch"
  else
    # Remove UTF-8 manifest
    echo >"$_GCC_DIR/gcc/config/i386/winnt-utf8.manifest"
  fi

  # Disable `_aligned_malloc`
  [[ $_WIN32_WINNT -lt 0x0501 ]] && patch -d "$_GCC_DIR" -Np1 <"$_PATCH_DIR/gcc/disable-aligned-malloc.patch"

  # development snapshot temporary fix
  fn-version-ge $_GCC_VER 15 && patch -d "$_GCC_DIR" -Np1 <"$_PATCH_DIR/gcc/15-fix-mingw-libs.patch"
fi

## GCC deps: GMP, MPFR, MPC, libiconv, gettext
[[ -d "$_GMP_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_GMP_ARCHIVE" --no-same-owner
[[ -d "$_MPFR_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_MPFR_ARCHIVE" --no-same-owner
[[ -d "$_MPC_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_MPC_ARCHIVE" --no-same-owner
[[ -d "$_ICONV_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_ICONV_ARCHIVE" --no-same-owner
[[ -d "$_GETTEXT_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_GETTEXT_ARCHIVE" --no-same-owner

## GDB
if [[ ! -d "$_GDB_DIR" ]]; then
  bsdtar -xf "$_ASSETS_DIR/$_GDB_ARCHIVE" --no-same-owner

  # Fix XP Vista x64
  [[ $_HOST_WINNT -lt 0x0601 ]] && patch -d "$_GDB_DIR" -Np1 <"$_PATCH_DIR/gdb/fix-xp-vista-x64.patch"

  if [[ $_HOST_WINNT -lt 0x0501 ]]; then
    cp "$_PATCH_DIR/gdb/win32-thunk.h" "$_GDB_DIR/gdb/win32-thunk.h"

    # Kernel32 thunk
    patch -d "$_GDB_DIR" -Np1 <"$_PATCH_DIR/gdb/kernel32-thunk.patch"

    # IPv6 thunk
    patch -d "$_GDB_DIR" -Np1 <"$_PATCH_DIR/gdb/ipv6-thunk.patch"
  fi
fi

## Make
[[ -d "$_MAKE_DIR" ]] || bsdtar -xf "$_ASSETS_DIR/$_MAKE_ARCHIVE" --no-same-owner
