#include <thunk/_common.h>
#include <thunk/_no_thunk.h>

#include <stdio.h>

#include "@console_buffer.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(
      api_ms_win_crt_stdio_l1_1_0, 0, int, __cdecl, fputc, int c, FILE *stream)
  {
    int fd = _fileno(stream);
    if (!internal::is_console(fd))
      return __ms_fputc(c, stream);

    unsigned char ch = c;
    auto &buffer = internal::stdio_buffer[fd];
    buffer.push(ch);

    if (internal::is_buffered(fd)) {
      if (ch == '\n')
        buffer.flush_all(fd);
    } else {
      buffer.flush_complete_sequence(fd);
    }
    return ch;
  }
} // namespace mingw_thunk
