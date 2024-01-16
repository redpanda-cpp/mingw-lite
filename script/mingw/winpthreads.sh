#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_MINGW_DIR/mingw-w64-libraries/winpthreads/build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_PREFIX/$_TARGET" \
  --with-sysroot="$_PREFIX/$_TARGET" \
  --host="$_TARGET" \
  --enable-static \
  --disable-shared \
  CFLAGS="-Os" \
  LDFLAGS="-s"
make -j$(nproc)
make DESTDIR="$_PKG_DIR" install
