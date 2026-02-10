target("openmp")
  set_kind("phony")
  add_cxflags("-fopenmp", {public = true})
  add_links("gomp", "pthread", {public = true})

target("openmp/hello-c")
  set_languages("c11")
  add_deps("openmp")
  add_files("hello.c")
  add_tests("default", {
    pass_outputs =
      "Hello, world!\n" ..
      "Hello, world!\n" ..
      "Hello, world!\n" ..
      "Hello, world!\n"})

target("openmp/hello-c++")
  set_languages("c++17")
  add_deps("openmp")
  add_files("hello.cc")
  add_tests("default", {
    pass_outputs =
      "Hello, world!\n" ..
      "Hello, world!\n" ..
      "Hello, world!\n" ..
      "Hello, world!\n"})

target("openmp/atomic-c")
  set_languages("c11")
  add_deps("openmp")
  add_files("atomic.c")
  add_tests("default", {pass_outputs = "g = 10000000\n"})

target("openmp/atomic-c++")
  set_languages("c++17")
  add_deps("openmp")
  add_files("atomic.cc")
  add_tests("default", {pass_outputs = "g = 10000000\n"})
