function enable_if_c_macro(macro, opt)
  local optname = "__enable_if_c_macro_" .. macro
  interp_save_scope()
  option(optname)
    set_showmenu(false)
    local snippet = ([[
      #if defined(%s)
      #else
        #error %s is not satisfied!
      #endif
    ]]):format(macro, macro)
    add_csnippets(macro, snippet)
    if opt.languages then
      set_languages(opt.languages)
    end
    if opt.cflags then
      add_cflags(opt.cflags)
    end
    if opt.cxflags then
      add_cxxflags(opt.cxflags)
    end
  option_end()
  interp_restore_scope()
  set_enabled(get_config(optname))
end

function enable_if_cxx_feature(macro, minvalue, opt)
  local optname = "__enable_if_cxx_feature_" .. macro .. "_" .. minvalue
  interp_save_scope()
  option(optname)
    set_showmenu(false)
    local snippet = ([[
      #include <version>
      #if %s >= %d
      #else
        #error %s >= %d is not satisfied!
      #endif
      ]]):format(macro, minvalue, macro, minvalue)
    add_cxxsnippets(macro, snippet)
    if opt.languages then
      set_languages(opt.languages)
    end
    if opt.cxflags then
      add_cflags(opt.cxflags)
    end
    if opt.cxxflags then
      add_cxxflags(opt.cxxflags)
    end
  option_end()
  interp_restore_scope()
  set_enabled(get_config(optname))
end

function enable_if_cxx_header(header)
  local optname = "__enable_if_cxx_header_" .. header
  interp_save_scope()
  option(optname)
    set_showmenu(false)
    add_cxxincludes(header)
  option_end()
  interp_restore_scope()
  set_enabled(get_config(optname))
end
