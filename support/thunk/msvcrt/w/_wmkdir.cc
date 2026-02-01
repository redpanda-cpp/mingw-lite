#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <direct.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _wmkdir, const wchar_t *dirname)
  {
    if (internal::is_nt())
      return __ms__wmkdir(dirname);

    stl::string a_name = internal::w2a(dirname);
    return __ms__mkdir(a_name.c_str());
  }
} // namespace mingw_thunk
