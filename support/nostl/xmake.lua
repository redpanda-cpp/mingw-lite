add_rules('mode.debug', 'mode.release', 'mode.minsizerel')
set_languages('c11', 'c++14')

includes('dep/catch2')

target('nocrt')
    set_kind('headeronly')
    if is_os('windows') then
        set_enabled(true)
    else
        set_enabled(false)
    end
    add_headerfiles('include/(nocrt/**.*)')
    add_includedirs('include', {public = true})

target('nostl')
    set_kind('headeronly')
    add_headerfiles('include/(nostl/**.*)')
    add_includedirs('include', {public = true})

target('test/nocrt')
    set_kind('binary')
    add_deps('catch2', 'nocrt')
    set_default(false)
    if is_os('windows') then
        set_enabled(true)
    else
        set_enabled(false)
    end

    add_files('test/nocrt/**.cc')
    add_ldflags('-static')
    add_tests('default')

target('test/nostl')
    set_kind('binary')
    add_deps('catch2', 'nostl')
    set_default(false)

    add_files('test/nostl/**.cc')
    add_ldflags('-static')
    add_tests('default')

target('test/nostl-nocrt')
    set_kind('binary')
    add_deps('catch2', 'nostl', 'nocrt')
    set_default(false)
    if is_os('windows') then
        set_enabled(true)
    else
        set_enabled(false)
    end

    add_defines('NOSTL_NOCRT')
    add_files('test/nostl/**.cc')
    add_ldflags('-static')
    add_tests('default')
