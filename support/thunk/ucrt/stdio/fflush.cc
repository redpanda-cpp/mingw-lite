#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/utf8-musl.h>

#include <stdio.h>

#include "@stdio.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(
      api_ms_win_crt_stdio_l1_1_0, 0, int, __cdecl, fflush, FILE *stream)
  {
    if (!stream) {
      if (i::is_console(1))
        musl::fflush(musl::g_stdout);
      return __ms_fflush(nullptr);
    }

    int fd = _fileno(stream);
    if (!i::is_console(fd))
      return __ms_fflush(stream);

    return musl::fflush(musl::g_fp_from_fd(fd));
  }
} // namespace mingw_thunk
