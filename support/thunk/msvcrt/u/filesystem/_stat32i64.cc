#include <thunk/_common.h>
#include <thunk/string.h>
#include <thunk/wntcrt/stat.h>

#include <errno.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _stat32i64,
                 const char *path,
                 struct _stat32i64 *buffer)
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

    return _wstat32i64(w_path.c_str(), buffer);
  }

  __DECLARE_FORCE_OVERRIDE_MINGW_EMU(_stat32i64)
} // namespace mingw_thunk
