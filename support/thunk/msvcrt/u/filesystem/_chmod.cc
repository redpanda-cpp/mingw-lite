#include <thunk/_common.h>
#include <thunk/string.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _chmod, const char *path, int mode)
  {
    stl::wstring w_path = internal::u2w(path);
    return _wchmod(w_path.c_str(), mode);
  }
} // namespace mingw_thunk
