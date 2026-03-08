#include <thunk/_common.h>
#include <thunk/string.h>

#include <errno.h>
#include <stdio.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 rename,
                 const char *old_filename,
                 const char *new_filename)
  {
    if (!old_filename || !new_filename) {
      _set_errno(EINVAL);
      return -1;
    }

    d::w_str w_old;
    if (!w_old.from_u(old_filename)) {
      _set_errno(ENOMEM);
      return -1;
    }

    d::w_str w_new;
    if (!w_new.from_u(new_filename)) {
      _set_errno(ENOMEM);
      return -1;
    }

    return _wrename(w_old.c_str(), w_new.c_str());
  }
} // namespace mingw_thunk
