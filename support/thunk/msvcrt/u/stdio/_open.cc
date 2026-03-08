#include <thunk/_common.h>
#include <thunk/string.h>

#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdarg.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, _open, const char *path, int flags, ...)
  {
    if (!path) {
      _set_errno(EINVAL);
      return -1;
    }

    va_list args;
    va_start(args, flags);
    int mode = va_arg(args, int);
    va_end(args);

    d::w_str w_path;
    if (!w_path.from_u(path)) {
      _set_errno(ENOMEM);
      return -1;
    }

    return _wopen(w_path.c_str(), flags, mode);
  }
} // namespace mingw_thunk
