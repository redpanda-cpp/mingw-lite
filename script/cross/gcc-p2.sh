#!/bin/bash

set -euxo pipefail

build_dir="$_BUILD_DIR/$_GCC_DIR/x-build"
cd "$build_dir"
make -j$(nproc)
make install
