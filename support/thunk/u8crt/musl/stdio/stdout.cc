#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    static unsigned char buf[BUFSIZ + UNGET];

    hidden FILE __stdout_FILE = {
        .flags = F_PERM | F_NORD,
        .close = __console_close,
        .write = __console_write,
        .seek = __console_seek,
        .buf = buf + UNGET,
        .buf_size = sizeof buf - UNGET,
        .fd = 1,
        .lock = -1,
        .lbf = '\n',
    };

    FILE *g_stdout = &__stdout_FILE;
  }; // namespace musl
} // namespace mingw_thunk
