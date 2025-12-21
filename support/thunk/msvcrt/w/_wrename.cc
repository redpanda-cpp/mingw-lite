#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _wrename,
                 const wchar_t *oldpath,
                 const wchar_t *newpath)
  {
    if (internal::is_nt())
      return __ms__wrename(oldpath, newpath);

    stl::string a_old = internal::w2a(oldpath);
    stl::string a_new = internal::w2a(newpath);
    return __ms_rename(a_old.c_str(), a_new.c_str());
  }
} // namespace mingw_thunk
