#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_BINUTILS_DIR/build"
mkdir -p "$build_dir" && cd "$build_dir"
../configure \
  --prefix="$_PREFIX" \
  --host="$_TARGET" \
  --target="$_TARGET" \
  --disable-nls \
  --with-static-standard-libraries \
  CFLAGS="-Os" \
  LDFLAGS="-s"
make MAKEINFO=true -j$(nproc)
make MAKEINFO=true install
