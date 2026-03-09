#include "_wrename.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
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
    __DISPATCH_THUNK_2(
        _wrename, i::is_nt(), &__ms__wrename, &f::win9x__wrename);

    return dllimport__wrename(oldpath, newpath);
  }

  namespace f
  {
    int win9x__wrename(const wchar_t *oldpath, const wchar_t *newpath)
    {
      if (!oldpath || !newpath) {
        _set_errno(EINVAL);
        return -1;
      }

      d::a_str a_old;
      if (!a_old.from_w(oldpath)) {
        _set_errno(ENOMEM);
        return -1;
      }

      d::a_str a_new;
      if (!a_new.from_w(newpath)) {
        _set_errno(ENOMEM);
        return -1;
      }

      return __ms_rename(a_old.c_str(), a_new.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
