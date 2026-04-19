#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/utf8-musl.h>

#include <stdio.h>

#include "@stdio.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_stdio_l1_1_0,
                 0,
                 char *,
                 __cdecl,
                 fgets,
                 char *s,
                 int n,
                 FILE *stream)
  {
    int fd = _fileno(stream);
    if (!i::is_console(fd))
      return __ms_fgets(s, n, stream);

    return musl::fgets(s, n, musl::g_fp_from_fd(fd));
  }
} // namespace mingw_thunk
