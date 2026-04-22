#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/utf8-musl.h>

#include <stdio.h>

#include "@stdio.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_stdio_l1_1_0, 0, int, __cdecl, getchar)
  {
    if (!i::is_console(0))
      return __ms_getchar();

    return musl::fgetc(musl::g_stdin);
  }
} // namespace mingw_thunk
