#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <direct.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _wchdir, const wchar_t *dirname)
  {
    if (internal::is_nt())
      return __ms__wchdir(dirname);

    stl::string a_name = internal::w2a(dirname);
    return __ms__chdir(a_name.c_str());
  }
} // namespace mingw_thunk
