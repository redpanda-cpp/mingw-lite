#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    static unsigned char buf[UNGET];

    hidden FILE __stderr_FILE = {
        .flags = F_PERM | F_NORD,
        .close = __console_close,
        .write = __console_write,
        .seek = __console_seek,
        .buf = buf + UNGET,
        .buf_size = 0,
        .fd = 2,
        .lock = -1,
        .lbf = -1,
    };

    FILE *g_stderr = &__stderr_FILE;
  }; // namespace musl
} // namespace mingw_thunk
