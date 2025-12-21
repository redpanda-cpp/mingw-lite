#include <thunk/_common.h>

#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_environment_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 putenv,
                 const char *string)
  {
    return _putenv(string);
  }
} // namespace mingw_thunk
