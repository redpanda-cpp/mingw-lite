#include <thunk/_common.h>

#include <direct.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_stdio_l1_1_0,
                 0,
                 char *,
                 __cdecl,
                 getcwd,
                 char *buf,
                 int size)
  {
    return _getcwd(buf, size);
  }
} // namespace mingw_thunk
