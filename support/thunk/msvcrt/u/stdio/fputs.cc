#include <thunk/_common.h>
#include <thunk/_no_thunk.h>

#include <stdio.h>

#include "@console_buffer.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, fputs, const char *s, FILE *stream)
  {
    int fd = _fileno(stream);
    if (!internal::is_console(fd))
      return __ms_fputs(s, stream);

    size_t len = strlen(s);
    auto &buffer = internal::stdio_buffer[fd];
    buffer.append(s, len);

    if (internal::is_buffered(fd)) {
      buffer.flush_complete_line(fd);
    } else {
      buffer.flush_complete_sequence(fd);
    }
    return len;
  }
} // namespace mingw_thunk
