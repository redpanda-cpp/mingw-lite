#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_MINGW_DIR/mingw-w64-libraries/winpthreads/x-build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_X_DIR/$_TARGET" \
  --with-sysroot="$_X_DIR/$_TARGET" \
  --host="$_TARGET" \
  --enable-static \
  --disable-shared \
  CFLAGS="-Os" \
  LDFLAGS="-s"
make -j$(nproc)
make install
