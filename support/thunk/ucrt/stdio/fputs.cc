#include <thunk/_common.h>
#include <thunk/_no_thunk.h>

#include <stdio.h>

#include "@console_buffer.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_stdio_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 fputs,
                 const char *s,
                 FILE *stream)
  {
    int fd = _fileno(stream);
    if (!i::is_console(fd))
      return __ms_fputs(s, stream);

    size_t len = strlen(s);
    auto &buffer = g::stdio_buffer[fd];
    buffer.append(s, len);

    buffer.flush_if_reaching_threshold(fd);

    if (i::is_buffered(fd)) {
      buffer.flush_complete_line(fd);
    } else {
      buffer.flush_complete_sequence(fd);
    }
    return len;
  }
} // namespace mingw_thunk
