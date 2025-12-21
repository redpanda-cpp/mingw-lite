#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/unicode.h>

#include <stdio.h>
#include <string.h>

#include "@console_buffer.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, puts, const char *s)
  {
    int fd = _fileno(stdout);
    if (!internal::is_console(fd))
      return __ms_puts(s);

    size_t len = strlen(s);
    auto &buffer = internal::stdio_buffer[fd];
    buffer.append(s, len);
    buffer.push('\n');
    buffer.flush_all(fd);
    return len + 1;
  }
} // namespace mingw_thunk
