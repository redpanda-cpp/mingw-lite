set_plat("mingw")
set_arch("x86_64")
add_rules("mode.debug", "mode.release")
set_languages("c11")

add_defines('DEBUG_BUILD_DIR="build/debug"')
add_defines('ENABLE_SHARED')
add_defines('ENABLE_UTF8')
add_defines('MINGW_DIR="mingw64"')
add_defines('XMAKE_ARCH="x86_64"')
add_ldflags('-municode', {force = true})

target("set-path")
  add_files("common.c", "set-path.c")

target("test-compiler")
  add_files("common.c", "test-compiler.c")

target("test-make-gdb")
  add_files("common.c", "test-make-gdb.c")

target("test-shared")
  add_files("common.c", "test-shared.c")
