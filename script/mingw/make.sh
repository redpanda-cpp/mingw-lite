#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_MAKE_DIR/build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_PREFIX" \
  --host="$_TARGET" \
  --disable-nls \
  CFLAGS="-Os" \
  LDFLAGS="-s"
make -j$(nproc)
cp make.exe "$_PREFIX/bin/mingw32-make.exe"
