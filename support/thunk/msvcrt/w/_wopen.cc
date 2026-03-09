#include "_wopen.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdarg.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, _wopen, const wchar_t *path, int flags, ...)
  {
    va_list args;
    va_start(args, flags);
    int mode = va_arg(args, int);
    va_end(args);

    __DISPATCH_THUNK_2(_wopen, i::is_nt(), &__ms__wopen, &f::win9x__wopen);

    return dllimport__wopen(path, flags, mode);
  }

  namespace f
  {
    int win9x__wopen(const wchar_t *path, int flags, ...)
    {
      if (!path) {
        _set_errno(EINVAL);
        return -1;
      }

      va_list args;
      va_start(args, flags);
      int mode = va_arg(args, int);
      va_end(args);

      d::a_str a_path;
      if (!a_path.from_w(path)) {
        _set_errno(ENOMEM);
        return -1;
      }

      return __ms__open(a_path.c_str(), flags, mode);
    }
  } // namespace f
} // namespace mingw_thunk
