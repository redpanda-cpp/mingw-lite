#include <thunk/_common.h>

#include <stdio.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      api_ms_win_crt_stdio_l1_1_0, 0, int, __cdecl, putc, int c, FILE *stream)
  {
    return fputc(c, stream);
  }
} // namespace mingw_thunk
