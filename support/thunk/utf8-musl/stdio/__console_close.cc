#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    hidden int __console_close(FILE *f)
    {
      if (f->u8_len > 0) {
        f->u8_len = 0;
      }
      return 0;
    }
  } // namespace musl
} // namespace mingw_thunk
