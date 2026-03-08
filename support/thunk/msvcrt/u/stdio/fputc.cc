#include <thunk/_common.h>
#include <thunk/_no_thunk.h>

#include <stdio.h>

#include "@console_buffer.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, fputc, int c, FILE *stream)
  {
    int fd = _fileno(stream);
    if (!i::is_console(fd))
      return __ms_fputc(c, stream);

    unsigned char ch = c;
    auto &buffer = g::stdio_buffer[fd];
    buffer.push(ch);

    buffer.flush_if_reaching_threshold(fd);

    if (i::is_buffered(fd)) {
      if (ch == '\n')
        buffer.flush_all(fd);
    } else {
      buffer.flush_complete_sequence(fd);
    }
    return ch;
  }
} // namespace mingw_thunk
