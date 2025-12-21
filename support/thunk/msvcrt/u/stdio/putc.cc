#include <thunk/_common.h>

#include <stdio.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, putc, int c, FILE *stream)
  {
    return fputc(c, stream);
  }
} // namespace mingw_thunk
