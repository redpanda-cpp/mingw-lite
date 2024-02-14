#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_GCC_DIR/build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix=$_PREFIX \
  --with-sysroot=$_PREFIX/$_TARGET \
  --with-native-system-header-dir=/include \
  --target=$_TARGET \
  --host=$_TARGET \
  --enable-static \
  --disable-shared \
  --with-pic \
  --with-gmp-include="$_DEP_DIR/include" \
  --with-gmp-lib="$_DEP_DIR/lib" \
  --with-mpc-include="$_DEP_DIR/include" \
  --with-mpc-lib="$_DEP_DIR/lib" \
  --with-mpfr-include="$_DEP_DIR/include" \
  --with-mpfr-lib="$_DEP_DIR/lib" \
  --with-libiconv-prefix="$_DEP_DIR" \
  --enable-languages=c,c++ \
  --enable-libgomp \
  --enable-threads=posix \
  --enable-version-specific-runtime-libs \
  --disable-dependency-tracking \
  --disable-multilib \
  --enable-nls \
  --disable-win32-registry \
  --enable-mingw-wildcard \
  CFLAGS_FOR_TARGET="-Os" \
  CXXFLAGS_FOR_TARGET="-Os" \
  LDFLAGS_FOR_TARGET="-s" \
  CFLAGS="-Os" \
  CXXFLAGS="-Os" \
  LDFLAGS="-s"
make -j$(nproc)
make DESTDIR="$_PKG_DIR" install
