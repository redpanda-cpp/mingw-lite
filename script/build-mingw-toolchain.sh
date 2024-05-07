#!/bin/bash

set -euxo pipefail

function title() {
  echo -ne "\033]0;$1\007"
}

export PATH="$_X_DIR/bin:$PATH"

title "Building MinGW binutils"
"$_SCRIPT_DIR/mingw/binutils.sh"

title "Building MinGW GCC prerequisites - GMP"
"$_SCRIPT_DIR/mingw/gmp.sh"

title "Building MinGW GCC prerequisites - MPFR"
"$_SCRIPT_DIR/mingw/mpfr.sh"

title "Building MinGW GCC prerequisites - MPC"
"$_SCRIPT_DIR/mingw/mpc.sh"

title "Building MinGW headers"
"$_SCRIPT_DIR/mingw/headers.sh"

title "Building MinGW CRT"
"$_SCRIPT_DIR/mingw/crt.sh"

title "Building MinGW winpthreads"
"$_SCRIPT_DIR/mingw/winpthreads.sh"

if [[ "$_THREAD" == "mcf" ]]; then
  title "Building MinGW MCF Gthread"
  "$_SCRIPT_DIR/mingw/mcf.sh"
fi

[[ -L "$_PREFIX" ]] || ln -s "$_PKG_DIR$_PREFIX" "$_PREFIX"

title "Building MinGW libiconv"
"$_SCRIPT_DIR/mingw/iconv.sh"

title "Building MinGW gettext"
"$_SCRIPT_DIR/mingw/gettext.sh"

title "Building MinGW GCC"
"$_SCRIPT_DIR/mingw/gcc.sh"

title "Building MinGW GDB"
"$_SCRIPT_DIR/mingw/gdb.sh"

title "Building MinGW Make"
"$_SCRIPT_DIR/mingw/make.sh"

title "Copying licenses"
"$_SCRIPT_DIR/mingw/license.sh"
