target("intl/γεια σου")
  add_files("γεια σου.c")
  add_tests("default", {pass_outputs = "Γεια σου, κόσμε!\n"})
  set_enabled(has_config("utf8"))

target("intl/привет")
  add_files("привет.c")
  add_tests("default", {pass_outputs = "Привет, мир!\n"})
  set_enabled(has_config("utf8"))

target("intl/مرحبا")
  add_files("مرحبا.c")
  add_tests("default", {pass_outputs = "مرحبا بالعالم!\n"})
  set_enabled(has_config("utf8"))

target("intl/नमस्ते")
  add_files("नमस्ते.c")
  add_tests("default", {pass_outputs = "नमस्ते, दुनिया!\n"})
  set_enabled(has_config("utf8"))

target("intl/‘hello’")
  add_files("‘hello’.c")
  add_tests("default", {pass_outputs = "Hello, world!\n"})

target("intl/こんにちは")
  add_files("こんにちは.c")
  add_tests("default", {pass_outputs = "こんにちは、世界！\n"})
  set_enabled(has_config("utf8"))

target("intl/你好")
  add_files("你好.c")
  add_tests("default", {pass_outputs = "你好，世界！\n"})
  set_enabled(has_config("utf8"))
