#!/bin/bash

set -euxo pipefail

src_dir="$_BUILD_DIR/$_MCFGTHREAD_DIR"
cd "$src_dir"

meson setup --cross-file "meson.cross.$_TARGET" x-build
ninja -C x-build libmcfgthread.a

install -Dm644 -t "$_X_DIR/$_TARGET/lib" x-build/libmcfgthread.a
install -Dm644 -t "$_X_DIR/$_TARGET/include/mcfgthread" mcfgthread/*.h mcfgthread/*.hpp x-build/version.h
