target("iconv/normal")
  -- set_encodings("target:cp1252") does not work
  add_cxflags("-fexec-charset=cp1252")
  add_files("normal.c")
  add_tests("default", {pass_outputs = "\x91hello\x92\n"})

target("iconv/error")
  add_cxflags("-fexec-charset=cp1252")
  add_files("error.c")
  add_tests("default", {build_should_fail = true})
  set_default(false)
