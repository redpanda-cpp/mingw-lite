#include <thunk/_common.h>

#include <fcntl.h>
#include <io.h>
#include <stdarg.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_stdio_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 open,
                 const char *path,
                 int flags,
                 ...)
  {
    if (flags & _O_CREAT) {
      va_list args;
      va_start(args, flags);
      int res = _open(path, flags, va_arg(args, int));
      va_end(args);
      return res;
    } else {
      return _open(path, flags);
    }
  }
} // namespace mingw_thunk
