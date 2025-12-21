#include <thunk/_common.h>
#include <thunk/string.h>

#include <stdio.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, remove, const char *path)
  {
    stl::wstring w_path = internal::u2w(path);
    return _wremove(w_path.c_str());
  }
} // namespace mingw_thunk
