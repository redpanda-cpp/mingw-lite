#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _wremove, const wchar_t *path)
  {
    if (internal::is_nt())
      return __ms__wremove(path);

    stl::string a_path = internal::w2a(path);
    return __ms_remove(a_path.c_str());
  }
} // namespace mingw_thunk
