#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    int fgetc(FILE *f)
    {
      FLOCK(f);
      int c = getc_unlocked(f);
      FUNLOCK(f);
      return c;
    }
  } // namespace musl
} // namespace mingw_thunk
