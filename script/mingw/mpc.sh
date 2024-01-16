#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_MPC_DIR/build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_DEP_DIR" \
  --host="$_TARGET" \
  --with-gmp-include="$_DEP_DIR/include" \
  --with-gmp-lib="$_DEP_DIR/lib" \
  --with-mpfr-include="$_DEP_DIR/include" \
  --with-mpfr-lib="$_DEP_DIR/lib" \
  --enable-static \
  --disable-shared \
  CFLAGS="-Os" \
  LDFLAGS="-s"
make -j$(nproc)
make install
