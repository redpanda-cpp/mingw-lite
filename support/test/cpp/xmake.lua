target("cpp/‘include’")
  add_files("‘include’.c")
  add_includedirs("..")
  add_tests("default", {
    pass_outputs =
      "Hello, world!\n" ..
      ".*‘include’%.c\n"})

target("cpp/शामिल करें")
  add_files("शामिल करें.c")
  add_includedirs("..")
  add_tests("default", {
    pass_outputs =
      "Hello, world!\n" ..
      ".*शामिल करें%.c\n"})
  set_enabled(has_config("utf8"))
