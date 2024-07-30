#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_GDB_DIR/build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_PREFIX" \
  --host="$_TARGET" \
  --disable-tui \
  CFLAGS="-Os -DPDC_WIDE -I$_DEP_DIR/include" \
  CXXFLAGS="-Os -DPDC_WIDE -I$_DEP_DIR/include" \
  LDFLAGS="-s -L$_DEP_DIR/lib"
make MAKEINFO=true -j$(nproc)
make install
