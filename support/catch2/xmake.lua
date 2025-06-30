target('catch2')
  set_kind('static')
  add_includedirs('include', {public = true})
  add_files('src/catch_amalgamated.cpp')
