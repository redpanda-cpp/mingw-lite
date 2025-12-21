#include <thunk/_common.h>
#include <thunk/string.h>

#include <stdio.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, rename, const char *oldpath, const char *newpath)
  {
    stl::wstring w_old = internal::u2w(oldpath);
    stl::wstring w_new = internal::u2w(newpath);
    return _wrename(w_old.c_str(), w_new.c_str());
  }
} // namespace mingw_thunk
