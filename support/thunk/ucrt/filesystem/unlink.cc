#include <thunk/_common.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 unlink,
                 const char *path)
  {
    return _unlink(path);
  }
} // namespace mingw_thunk
