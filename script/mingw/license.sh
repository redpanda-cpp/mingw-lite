#!/bin/bash

set -euxo pipefail

license_dir="$_PKG_DIR/$_PREFIX/license"

install -Dm644 "$_BUILD_DIR/$_BINUTILS_DIR/COPYING" "$license_dir/binutils.txt"
install -Dm644 "$_BUILD_DIR/$_BINUTILS_DIR/COPYING.LIB" "$license_dir/binutils-lib.txt"
install -Dm644 "$_BUILD_DIR/$_BINUTILS_DIR/COPYING3" "$license_dir/binutils3.txt"
install -Dm644 "$_BUILD_DIR/$_BINUTILS_DIR/COPYING3.LIB" "$license_dir/binutils3-lib.txt"

install -Dm644 "$_BUILD_DIR/$_GMP_DIR/COPYING" "$license_dir/gmp.txt"
install -Dm644 "$_BUILD_DIR/$_GMP_DIR/COPYINGv2" "$license_dir/gmpv2.txt"
install -Dm644 "$_BUILD_DIR/$_GMP_DIR/COPYING.LESSERv3" "$license_dir/gmp-lesserv3.txt"

install -Dm644 "$_BUILD_DIR/$_MPFR_DIR/COPYING" "$license_dir/mpfr.txt"
install -Dm644 "$_BUILD_DIR/$_MPFR_DIR/COPYING.LESSER" "$license_dir/mpfr-lesser.txt"

install -Dm644 "$_BUILD_DIR/$_MPC_DIR/COPYING.LESSER" "$license_dir/mpc.txt"

install -Dm644 "$_BUILD_DIR/$_MINGW_DIR/COPYING" "$license_dir/mingw-w64.txt"

install -Dm644 "$_BUILD_DIR/$_MINGW_DIR/mingw-w64-libraries/winpthreads/COPYING" "$license_dir/winpthreads.txt"

if [[ "$_THREAD" == "mcf" ]]; then
  install -Dm644 "$_BUILD_DIR/$_MCFGTHREAD_DIR/LICENSE.TXT" "$license_dir/mcfgthread.txt"
fi

install -Dm644 "$_BUILD_DIR/$_ICONV_DIR/COPYING" "$license_dir/libiconv.txt"
install -Dm644 "$_BUILD_DIR/$_ICONV_DIR/COPYING.LIB" "$license_dir/libiconv-lib.txt"

install -Dm644 "$_BUILD_DIR/$_GETTEXT_DIR/gettext-runtime/intl/COPYING.LIB" "$license_dir/gettext-lib.txt"

install -Dm644 "$_BUILD_DIR/$_GCC_DIR/COPYING" "$license_dir/gcc.txt"
install -Dm644 "$_BUILD_DIR/$_GCC_DIR/COPYING.LIB" "$license_dir/gcc-lib.txt"
install -Dm644 "$_BUILD_DIR/$_GCC_DIR/COPYING.RUNTIME" "$license_dir/gcc-runtime.txt"
install -Dm644 "$_BUILD_DIR/$_GCC_DIR/COPYING3" "$license_dir/gcc3.txt"
install -Dm644 "$_BUILD_DIR/$_GCC_DIR/COPYING3.LIB" "$license_dir/gcc3-lib.txt"

install -Dm644 "$_BUILD_DIR/$_GDB_DIR/COPYING" "$license_dir/gdb.txt"
install -Dm644 "$_BUILD_DIR/$_GDB_DIR/COPYING.LIB" "$license_dir/gdb-lib.txt"
install -Dm644 "$_BUILD_DIR/$_GDB_DIR/COPYING3" "$license_dir/gdb3.txt"
install -Dm644 "$_BUILD_DIR/$_GDB_DIR/COPYING3.LIB" "$license_dir/gdb3-lib.txt"

install -Dm644 "$_BUILD_DIR/$_MAKE_DIR/COPYING" "$license_dir/make.txt"
