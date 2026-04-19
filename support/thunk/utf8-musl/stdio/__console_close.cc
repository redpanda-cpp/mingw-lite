#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    hidden int __console_close(FILE *f)
    {
      return 0;
    }
  } // namespace musl
} // namespace mingw_thunk
