#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <direct.h>
#include <wchar.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(int, _wchdir, const wchar_t *dirname)
  {
    if (internal::is_nt())
      return get__wchdir()(dirname);

    stl::string a_name = internal::narrow(dirname);
    return _chdir(a_name.c_str());
  }
} // namespace mingw_thunk
