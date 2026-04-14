#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    hidden off_t __console_seek(FILE *f, off_t off, int whence)
    {
      return -1;
    }
  } // namespace musl
} // namespace mingw_thunk
