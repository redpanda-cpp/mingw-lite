#!/bin/bash

set -euxo pipefail

src_dir="$_BUILD_DIR/$_MCFGTHREAD_DIR"
cd "$src_dir"

meson setup --cross-file "meson.cross.$_TARGET" build
ninja -C build libmcfgthread.a

install -Dm644 -t "$_PREFIX/$_TARGET/lib" build/libmcfgthread.a
install -Dm644 -t "$_PREFIX/$_TARGET/include/mcfgthread" mcfgthread/*.h mcfgthread/*.hpp build/version.h
