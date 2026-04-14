#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    static unsigned char buf[BUFSIZ + UNGET];

    hidden FILE __stdin_FILE = {
        .flags = F_PERM | F_NOWR,
        .close = __console_close,
        .read = __console_read,
        .seek = __console_seek,
        .buf = buf + UNGET,
        .buf_size = sizeof buf - UNGET,
        .fd = 0,
        .lock = -1,
    };

    FILE *g_stdin = &__stdin_FILE;
  }; // namespace musl
} // namespace mingw_thunk
