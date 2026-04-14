#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    int __uflow(FILE *f)
    {
      unsigned char c;
      if (!__toread(f) && f->read(f, &c, 1) == 1)
        return c;
      return EOF;
    }
  } // namespace musl
} // namespace mingw_thunk
