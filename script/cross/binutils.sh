#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_BINUTILS_DIR/x-build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_X_DIR" \
  --with-sysroot="$_X_DIR/$_TARGET" \
  --target="$_TARGET" \
  --disable-nls \
  --with-static-standard-libraries \
  --disable-multilib
make MAKEINFO=true -j$(nproc)
make MAKEINFO=true install
