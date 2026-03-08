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
    if (internal::is_nt())
      return __ms__wrename(oldpath, newpath);

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
} // namespace mingw_thunk
