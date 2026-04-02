# Internal Build Process

## To Cross, or Not to Cross

> See also [GCC configure terms](https://gcc.gnu.org/onlinedocs/gccint/Configure-Terms.html) for “native”, “cross”, “cross-built native”, “canadian”.

MinGW Lite is meant to delivery toolchains that runs on Windows and builds for Windows, as if it were a native toolchain.

The toolchains are, to some degree, canadian toolchain (build = x86_64-linux-gnu, host = x86_64-w64-mingw32_utf8_more_thunk, target = x86_64-w64-mingw32) – the toolchain itself (host) is built with UTF-8 thunks and more features thunks; but the programs it builds (target) is not.

Because adding new triplet to build systems is not trivial, and it costs much more time to build a canadian toolchain (2 cross toolchains are needed: one for final host, with build = host = x86_64-linux-gnu, target = x86_64-w64-mingw32_utf8_more_thunk; another for final target, with build = host = x86_64-linux-gnu, target = x86_64-w64-mingw32), MinGW Lite introduced a trick, identifying it as cross-built native (build = x86_64-linux-gnu, host = target = x86_64-w64-mingw32) instead of canadian.

## The “not to Cross” Trick

x86_64-w64-mingw32_utf8_more_thunk is quite similar to x86_64-w64-mingw32; the only difference is the CRT import libraries – the former one contains more thunks.

Thus, MinGW Lite uses same triplet for final host and target, x86_64-w64-mingw32. CRT import libraries are built twice, once with UTF-8 thunks and more features thunks for final host, and once without them for final target.

Static runtime libraries are simple and easy – they are not _linked_, so it does not matter which CRT they are built with.

Tools and shared runtime libraries are linked. In the build process, proper CRT is mounted to `/usr/local` (the `with overlayfs_ro('/usr/local', layers)` pattern). Tools (the toolchain itself) are host stuff, so they are built with host CRT. Since the toolchain itself is statically linked, shared runtime libraries can be treated as target stuff, so they are built with target CRT.

## The Build Process

### Phase 1: Build Tools

Internally named “AAA”, build = host = target = x86_64-linux-gnu.

[module/AAA.py](module/AAA.py):
- `build_AAA_library`
  - gmp
  - mpfr
  - mpc
  - zlib
- `build_AAA_tool`
  - python
  - setuptools
  - meson
  - xmake

### Phase 2: Cross Toolchain

Internally named “AAB”, build = host = x86_64-linux-gnu, target = x86_64-w64-mingw32.

[module/AAB.py](module/AAB.py):
- `build_AAB_compiler`
  - binutils
  - mingw-w64-headers
  - gcc (compiler)
  - mingw-w64-crt (host)
  - mingw-w64-crt (target)
  - UTF-8 tricks
  - i386 atomic bootstrap helper
  - mcfgthreads
  - winpthreads (bootstrap)
  - mingw-w64-headers (cleanup pthread dummy header)
  - gcc (runtime)
  - winpthreads
- `build_AAB_library`
  - gmp
  - mpfr
  - mpc
  - expat
  - iconv
  - intl
  - pdcurses
  - python

### Phase 3: Cross-Built Native Toolchain

Internally named “ABB”, build = x86_64-linux-gnu, host = target = x86_64-w64-mingw32.

[module/ABB.py](module/ABB.py):
- `build_ABB_test_driver`
- `build_ABB_toolchain`
  - binutils
  - mingw-w64-headers
  - mingw-w64-crt
  - winpthreads
  - mcfgthreads
  - gcc (compiler)
  - gcc (runtime)
  - gdb
  - make
  - pkgconf
- `build_ABB_xmake`
