#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_ICONV_DIR/build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_DEP_DIR" \
  --host="$_TARGET" \
  --disable-nls \
  --enable-static \
  --disable-shared \
  CFLAGS="-Os" \
  LDFLAGS="-s"
make -j$(nproc)
make install
