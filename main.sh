#!/bin/bash

set -euxo pipefail

export _BRANCH=""
export _BUILD_X_MINGW=1
export _CLEAN=0
export _INSTALL_DEPS=1
export _PROFILE=""
while [[ $# -gt 0 ]]; do
  case "$1" in
    -b|--branch)
      _BRANCH="$2"
      shift 2
      ;;
    -c|--clean)
      _CLEAN=1
      shift
      ;;
    -p|--profile)
      _PROFILE="$2"
      shift 2
      ;;
    -nx|--no-cross)
      _BUILD_X_MINGW=0
      shift
      ;;
    -nd|--no-deps)
      _INSTALL_DEPS=0
      shift
      ;;
    *)
      echo "Unknown option: $1"
      exit 1
      ;;
  esac
done

if [[ -f "profile/$_BRANCH/$_PROFILE.bash" ]]; then
  . "profile/$_BRANCH/$_PROFILE.bash"
else
  echo "Invalid profile: $_BRANCH/$_PROFILE"
  exit 1
fi

export _PROJECT_ROOT="$PWD"
export _ASSETS_DIR="$_PROJECT_ROOT/assets"
export _BUILD_DIR="/tmp/build/mingw$_PROFILE-$_BRANCH"
export _X_DIR="/opt/x-mingw$_PROFILE-$_BRANCH"
export _DEP_DIR="$_BUILD_DIR/deps"
export _DIST_DIR="$_PROJECT_ROOT/dist"
export _PATCH_DIR="$_PROJECT_ROOT/patch"
export _PKG_ARCHIVE="mingw$_PROFILE-$_GCC_VER-r$_REV"
export _X_ARCHIVE="x-mingw$_PROFILE-$_GCC_VER-r$_REV"
export _SCRIPT_DIR="$_PROJECT_ROOT/script"
export _PREFIX="/opt/mingw$_PROFILE-$_BRANCH"

export _BINUTILS_DIR="binutils-$_BINUTILS_VER"
export _MINGW_DIR="mingw-w64-v$_MINGW_VER"
export _MCFGTHREAD_DIR="mcfgthread-$_MCFGTHREAD_VER"
export _GCC_DIR="gcc-$_GCC_VER"
export _GMP_DIR="gmp-$_GMP_VER"
export _MPFR_DIR="mpfr-$_MPFR_VER"
export _MPC_DIR="mpc-$_MPC_VER"
export _ICONV_DIR="libiconv-$_ICONV_VER"
export _GETTEXT_DIR="gettext-$_GETTEXT_VER"
export _GDB_DIR="gdb-$_GDB_VER"
export _MAKE_DIR="make-$_MAKE_VER"

export _BINUTILS_ARCHIVE="$_BINUTILS_DIR.tar.xz"
export _MINGW_ARCHIVE="$_MINGW_DIR.tar.bz2"
export _MCFGTHREAD_ARCHIVE="$_MCFGTHREAD_DIR.tar.gz"
export _GCC_ARCHIVE="$_GCC_DIR.tar.xz"
export _GMP_ARCHIVE="$_GMP_DIR.tar.xz"
export _MPFR_ARCHIVE="$_MPFR_DIR.tar.xz"
export _MPC_ARCHIVE="$_MPC_DIR.tar.gz"
export _ICONV_ARCHIVE="$_ICONV_DIR.tar.gz"
export _GETTEXT_ARCHIVE="$_GETTEXT_DIR.tar.xz"
export _GDB_ARCHIVE="$_GDB_DIR.tar.xz"
export _MAKE_ARCHIVE="$_MAKE_DIR.tar.gz"

function install-deps() {
  [[ $_INSTALL_DEPS -eq 0 ]] && return
  apt update
  DEBIAN_FRONTEND=noninteractive apt install --no-install-recommends -y \
    bison build-essential ca-certificates curl file flex gawk gettext libarchive-tools libgmp-dev libmpc-dev libmpfr-dev m4 meson p7zip-full texinfo zstd
}

function clean() {
  [[ $_CLEAN -eq 0 ]] && return
  [[ -d "$_BUILD_DIR" ]] && rm -rf "$_BUILD_DIR"
  [[ $_BUILD_X_MINGW -eq 1 && -d "$_X_DIR" ]] && rm -rf "$_X_DIR"
  [[ -d "$_PREFIX" ]] && rm -rf "$_PREFIX"
  true
}

function prepare-dirs() {
  mkdir -p "$_ASSETS_DIR"
  mkdir -p "$_BUILD_DIR"
  mkdir -p "$_X_DIR"
  mkdir -p "$_DIST_DIR"
}

function package() {
  pushd /opt
  7z a -t7z -mf=BCJ2 -mx9 -m0=LZMA:d=64m -ms=on "$_DIST_DIR/$_PKG_ARCHIVE.7z" "mingw$_PROFILE-$_BRANCH"
  popd
  # good balance of speed and size (zstd 1.5)
  bsdtar -c "$_X_DIR" | zstd --zstd=strat=5,wlog=27,hlog=25,slog=6 -f -o "$_DIST_DIR/$_X_ARCHIVE.tar.zst"
}

install-deps
clean
prepare-dirs
script/prepare-sources.sh
[[ $_BUILD_X_MINGW -eq 1 ]] && script/build-cross-compiler.sh
script/build-mingw-toolchain.sh
package
