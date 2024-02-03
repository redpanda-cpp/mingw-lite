#!/bin/bash

set -euxo pipefail

export REV="1"

export BINUTILS_VER="2.41"
export MINGW_VER="11.0.1"
export GCC_VER="13.2.0"
export GMP_VER="6.3.0"
export MPFR_VER="4.2.1"
export MPC_VER="1.3.1"
export GDB_VER="14.1"
export MAKE_VER="4.4.1"

export _ARCH=""
export _CLEAN=0
export _CRT="msvcrt"
export _SKIP_DEPS=0
while [[ $# -gt 0 ]]; do
  case "$1" in
    --arch)
      _ARCH="$2"
      shift
      shift
      ;;
    --clean)
      _CLEAN=1
      shift
      ;;
    --crt)
      _CRT="$2"
      shift
      shift
      ;;
    --skip-deps)
      _SKIP_DEPS=1
      shift
      ;;
    *)
      echo "Unknown option: $1"
      exit 1
      ;;
  esac
done

case "$_ARCH" in
  32)
    export _TARGET="i686-w64-mingw32"
    export _CRT_CONFIG_FLAGS="--enable-lib32 --disable-lib64"
    ;;
  64)
    export _TARGET="x86_64-w64-mingw32"
    export _CRT_CONFIG_FLAGS="--disable-lib32 --enable-lib64"
    ;;
  *)
    echo "Please specify --arch 32 or --arch 64"
    exit 1
    ;;
esac

case "$_CRT" in
  msvcrt|msvcr80|msvcr90|msvcr100)
    if [[ "$_ARCH" == "64" ]]; then
      export _WIN32_WINNT="0x0502"
    else
      export _WIN32_WINNT="0x0501"
    fi
    ;;
  msvcr110|msvcr120|ucrt)
    export _WIN32_WINNT="0x0600"
    ;;
  *)
    echo "Please specify --crt msvcrt or --crt ucrt"
    exit 1
    ;;
esac

export _PROJECT_ROOT="$PWD"
export _RROFILE="mingw$_ARCH-$_CRT"
export _ASSETS_DIR="$_PROJECT_ROOT/assets"
export _BUILD_DIR="$_PROJECT_ROOT/build/$_RROFILE"
export _X_DIR="$_BUILD_DIR/x-tools"
export _DEP_DIR="$_BUILD_DIR/deps"
export _DIST_DIR="$_PROJECT_ROOT/dist"
export _PKG_DIR="$_PROJECT_ROOT/pkg/$_RROFILE"
export _SCRIPT_DIR="$_PROJECT_ROOT/script"
export _SRC_DIR="$_PROJECT_ROOT/src"
export _PREFIX="/mingw$_ARCH"

export _BINUTILS_DIR="binutils-$BINUTILS_VER"
export _MINGW_DIR="mingw-w64-v$MINGW_VER"
export _GCC_DIR="gcc-$GCC_VER"
export _GMP_DIR="gmp-$GMP_VER"
export _MPFR_DIR="mpfr-$MPFR_VER"
export _MPC_DIR="mpc-$MPC_VER"
export _GDB_DIR="gdb-$GDB_VER"
export _MAKE_DIR="make-$MAKE_VER"

export _BINUTILS_ARCHIVE="$_BINUTILS_DIR.tar.xz"
export _MINGW_ARCHIVE="$_MINGW_DIR.tar.bz2"
export _GCC_ARCHIVE="$_GCC_DIR.tar.xz"
export _GMP_ARCHIVE="$_GMP_DIR.tar.xz"
export _MPFR_ARCHIVE="$_MPFR_DIR.tar.xz"
export _MPC_ARCHIVE="$_MPC_DIR.tar.gz"
export _GDB_ARCHIVE="$_GDB_DIR.tar.xz"
export _MAKE_ARCHIVE="$_MAKE_DIR.tar.gz"

function install-deps() {
  [[ $_SKIP_DEPS -eq 1 ]] && return
  apt update
  DEBIAN_FRONTEND=noninteractive apt install --no-install-recommends -y \
    build-essential ca-certificates curl gawk libarchive-tools libgmp-dev libmpc-dev libmpfr-dev m4 p7zip-full texinfo
}

function clean() {
  rm "$_PREFIX" || true
  [[ $_CLEAN -eq 0 ]] && return
  rm -rf "$_BUILD_DIR" || true
  rm -rf "$_PKG_DIR" || true
}

function prepare-dirs() {
  mkdir -p "$_ASSETS_DIR"
  mkdir -p "$_BUILD_DIR"
  mkdir -p "$_DIST_DIR"
  mkdir -p "$_PKG_DIR"
  mkdir -p "$_SRC_DIR"
}

function package() {
  pushd "$_PKG_DIR"
  7z a -t7z -mf=BCJ2 -m0=LZMA:d=32m -ms=on "$_DIST_DIR/$_RROFILE-$GCC_VER-r$REV" mingw$_ARCH
  popd
}

install-deps
clean
prepare-dirs
script/prepare-sources.sh
script/build-cross-compiler.sh
script/build-mingw-toolchain.sh
package
