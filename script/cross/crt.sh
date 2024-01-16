#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_MINGW_DIR/mingw-w64-crt/x-build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_X_DIR/$_TARGET" \
  --with-sysroot="$_X_DIR/$_TARGET" \
  --host="$_TARGET" \
  --with-default-msvcrt="$_CRT" \
  --with-default-win32-winnt="$_WIN32_WINNT" \
  --disable-dependency-tracking \
  $_CRT_CONFIG_FLAGS \
  CFLAGS="-Os" \
  LDFLAGS="-s"
make -j$(nproc)
make install
