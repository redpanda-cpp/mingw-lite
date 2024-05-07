#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_GETTEXT_DIR/gettext-runtime/build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_DEP_DIR" \
  --host="$_TARGET" \
  --enable-static \
  --disable-shared \
  CFLAGS="-Os" \
  CXXFLAGS="-Os" \
  LDFLAGS="-s"
make -j$(nproc)
make install
