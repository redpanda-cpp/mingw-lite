target("tls/lib")
  add_files("lib.cc")
  set_enabled(has_config("dlopen"))
  set_filename("tls/lib.dll")
  set_languages("c++20")
  set_kind("shared")

target("tls/main")
  add_files("main.cc")
  add_tests("default", {
    pass_outputs =
      "[t1; start]\n" ..
      "[main; read tls] 42 0\n" ..
      "[t1; read tls] 42 0\n" ..
      "[t1; end]\n" ..
      "[t2; start]\n" ..
      "[t2; read tls] 42 0\n" ..
      "[t2; end]\n",
    plain = true,
  })
  set_languages("c++20")
  set_kind("binary")

  enable_if_cxx_feature("__cpp_lib_semaphore", 201907, {languages = "c++20"})

  -- override previous enable_if_cxx_feature
  if not has_config("dlopen") then
    set_enabled(false)
  end
