add_rules("mode.debug", "mode.release")
set_policy("build.c++.modules.gcc.cxx11abi", true)

includes("chimaera.lua")
includes("enable_if.lua")

target("c89/hello")
  add_files("c89/hello.c")
  add_tests("default", {pass_outputs = "Hello, world!\n"})
  chimaera()

target("c23/embed")
  enable_if_c_macro("__has_embed", {languages = "c23"})
  set_languages("c23")
  add_files("c23/embed.c")
  add_tests("default", {pass_outputs = "Hello, world!\n"})
  chimaera()

target("c++98/hello")
  add_files("c++98/hello.cc")
  add_tests("default", {pass_outputs = "Hello, world!\n"})
  chimaera()

target("gnu++98/bits-stdc++")
  add_files("gnu++98/bits-stdc++.cc")
  add_tests("default", {pass_outputs = "Hello, world!\n"})
  chimaera()

target("c++11/thread")
  set_languages("c++11")
  add_files("c++11/thread.cc")
  add_tests("default", {
    pass_outputs =
      "l = 1000000\n" ..
      "l = 1000000\n" ..
      "l = 1000000\n" ..
      "l = 1000000\n" ..
      "l = 1000000\n" ..
      "l = 1000000\n" ..
      "l = 1000000\n" ..
      "l = 1000000\n" ..
      "l = 1000000\n" ..
      "l = 1000000\n" ..
      "g = 10000000\n"})
  chimaera()

target("c++17/filesystem")
  enable_if_cxx_header("filesystem")
  set_languages("c++17")
  add_files("c++17/filesystem.cc")
  add_tests("default", {
    pass_outputs =
      "test__filesystem__‘dir_copy’\n" ..
      "test__filesystem__‘dir_copy’/‘file_copy’.txt\n" ..
      "test__filesystem__‘dir_copy’/‘file’.txt\n" ..
      "test__filesystem__‘dir’\n" ..
      "test__filesystem__‘dir’/‘file_copy’.txt\n" ..
      "test__filesystem__‘dir’/‘file’.txt\n"})
  chimaera()

target("c++17/fstream")
  enable_if_cxx_header("filesystem")
  set_languages("c++17")
  add_files("c++17/fstream.cc")
  add_tests("default", {pass_outputs = "Hello, world!\n"})
  chimaera()

target("c++23/print")
  enable_if_cxx_header("print")
  set_languages("c++23")
  add_files("c++23/print.cc")
  add_tests("default", {pass_outputs = "Hello, world!\n"})
  chimaera()

target("c++23/import-std")
  enable_if_cxx_feature("__cpp_lib_modules", 202207, {languages = "c++23", cxxflags = {"-fmodules"}})
  set_languages("c++23")
  set_policy("build.c++.modules", true)
  add_files("c++23/import-std.cc")
  add_tests("default", {pass_outputs = "Hello, world!\n"})
  chimaera()
