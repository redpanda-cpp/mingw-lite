#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/u8crt/musl.h>

#include <stdio.h>
#include <string.h>

#include "@stdio.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, puts, const char *s)
  {
    if (!i::is_console(1))
      return __ms_puts(s);

    size_t len = strlen(s);
    musl::fwrite(s, 1, len, musl::g_stdout);
    musl::fputc('\n', musl::g_stdout);
    return len + 1;
  }
} // namespace mingw_thunk
