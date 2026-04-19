#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/utf8-musl.h>

#include <stdio.h>

#include "@stdio.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, fputs, const char *s, FILE *stream)
  {
    int fd = _fileno(stream);
    if (!i::is_console(fd))
      return __ms_fputs(s, stream);

    size_t len = strlen(s);
    musl::fwrite(s, 1, len, musl::g_fp_from_fd(fd));
    return len;
  }
} // namespace mingw_thunk
