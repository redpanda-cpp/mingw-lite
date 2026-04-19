#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/utf8-musl.h>

#include <stdio.h>

#include "@stdio.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, ungetc, int c, FILE *stream)
  {
    int fd = _fileno(stream);
    if (!i::is_console(fd))
      return __ms_ungetc(c, stream);

    return musl::ungetc(c, musl::g_fp_from_fd(fd));
  }
} // namespace mingw_thunk
