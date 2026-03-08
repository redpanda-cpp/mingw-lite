#include <thunk/_common.h>
#include <thunk/string.h>

#include <errno.h>
#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _chmod, const char *path, int mode)
  {
    if (!path) {
      _set_errno(EINVAL);
      return -1;
    }

    d::w_str w_path;
    if (!w_path.from_u(path)) {
      _set_errno(ENOMEM);
      return -1;
    }

    return _wchmod(w_path.c_str(), mode);
  }
} // namespace mingw_thunk
