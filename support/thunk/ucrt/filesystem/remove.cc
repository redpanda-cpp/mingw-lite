#include <thunk/_common.h>
#include <thunk/string.h>

#include <errno.h>
#include <stdio.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 remove,
                 const char *path)
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

    return _wremove(w_path.c_str());
  }
} // namespace mingw_thunk
