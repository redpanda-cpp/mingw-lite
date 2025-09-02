set_plat("mingw")
set_arch("x86_64")
add_rules("mode.debug", "mode.release")
set_languages("c11")

add_defines('DEBUG_BUILD_DIR="build/debug"')
add_defines('MINGW_DIR="mingw64"')
add_defines('XMAKE_ARCH="x86_64"')

target("test-compiler")
  add_files("common.c", "test-compiler.c")

target("test-make-gdb")
  add_files("common.c", "test-make-gdb.c")
