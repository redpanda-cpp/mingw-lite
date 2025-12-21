#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _wunlink, const wchar_t *path)
  {
    if (internal::is_nt())
      return __ms__wunlink(path);

    stl::string a_path = internal::w2a(path);
    return __ms__unlink(a_path.c_str());
  }
} // namespace mingw_thunk
