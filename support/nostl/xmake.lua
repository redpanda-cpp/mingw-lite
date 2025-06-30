add_rules('mode.debug', 'mode.release', 'mode.minsizerel')
set_languages('c11', 'c++14')

includes('dep/catch2')

target('nostl')
    set_kind('headeronly')
    add_headerfiles('include/(nostl/**.*)')
    add_includedirs('include', {public = true})

target('test')
    set_default(false)
    set_kind('binary')
    add_packages()
    add_deps('catch2', 'nostl')
    add_files('test/**.cc')
    add_ldflags('-static')
    add_tests('test')

target('test-nocrt')
    set_default(false)
    if is_os('windows') then
        set_enabled(true)
    else
        set_enabled(false)
    end
    set_kind('binary')
    add_deps('catch2', 'nostl')
    add_defines('NOSTL_NOCRT')
    add_files('test/**.cc')
    add_ldflags('-static')
    add_tests('test')
