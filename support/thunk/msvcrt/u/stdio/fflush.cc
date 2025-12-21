#include <thunk/_common.h>
#include <thunk/_no_thunk.h>

#include <stdio.h>

#include "@console_buffer.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, fflush, FILE *stream)
  {
    int fd = _fileno(stream);
    if (!internal::is_console(fd))
      return __ms_fflush(stream);

    auto &buffer = internal::stdio_buffer[fd];
    buffer.flush_all(fd);
    return 0;
  }
} // namespace mingw_thunk
