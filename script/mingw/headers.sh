#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_MINGW_DIR/mingw-w64-headers/build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_PREFIX/$_TARGET" \
  --host="$_TARGET" \
  --with-default-msvcrt="$_CRT" \
  --with-default-win32-winnt="$_WIN32_WINNT"
make -j$(nproc)
make DESTDIR="$_PKG_DIR" install
