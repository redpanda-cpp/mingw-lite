#include <thunk/_common.h>
#include <thunk/string.h>

#include <errno.h>
#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _putenv, const char *string)
  {
    d::w_str w_string;
    if (!w_string.from_u(string)) {
      _set_errno(ENOMEM);
      return -1;
    }

    return _wputenv(w_string.c_str());
  }
} // namespace mingw_thunk
