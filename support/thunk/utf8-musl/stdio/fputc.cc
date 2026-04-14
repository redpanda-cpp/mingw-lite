#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    int fputc(int c, FILE *f)
    {
      FLOCK(f);
      c = putc_unlocked(c, f);
      FUNLOCK(f);
      return c;
    }
  } // namespace musl
} // namespace mingw_thunk
