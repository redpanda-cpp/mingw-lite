#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_GCC_DIR/x-build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_X_DIR" \
  --with-sysroot="$_X_DIR" \
  --target="$_TARGET" \
  --enable-static \
  --disable-shared \
  --with-pic \
  --enable-languages=c,c++ \
  --enable-libgomp \
  --enable-threads=posix \
  --enable-version-specific-runtime-libs \
  --disable-dependency-tracking \
  --disable-nls \
  --disable-multilib \
  CFLAGS_FOR_TARGET="-Os" \
  CXXFLAGS_FOR_TARGET="-Os" \
  LDFLAGS_FOR_TARGET="-s" \
  CFLAGS="-Os" \
  CXXFLAGS="-Os" \
  LDFLAGS="-s"
make -j$(nproc) all-gcc
make install-gcc
