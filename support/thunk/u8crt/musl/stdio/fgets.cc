#include "../internal/stdio_impl.h"

#include <string.h>

namespace mingw_thunk
{
  namespace musl
  {
    char *fgets(char *s, int n, FILE *f)
    {
      char *p = s;
      unsigned char *z;
      size_t k;
      int c;

      FLOCK(f);

      if (n <= 1) {
        f->mode |= f->mode - 1;
        FUNLOCK(f);
        if (n < 1)
          return 0;
        *s = 0;
        return s;
      }
      n--;

      while (n) {
        if (f->rpos != f->rend) {
          z = (unsigned char *)memchr(f->rpos, '\n', f->rend - f->rpos);
          k = z ? z - f->rpos + 1 : f->rend - f->rpos;
          if (k > (size_t)n)
            k = n;
          memcpy(p, f->rpos, k);
          f->rpos += k;
          p += k;
          n -= k;
          if (z || !n)
            break;
        }
        if ((c = getc_unlocked(f)) < 0) {
          if (p == s || !feof(f))
            s = 0;
          break;
        }
        n--;
        if ((*p++ = c) == '\n')
          break;
      }
      if (s)
        *p = 0;

      FUNLOCK(f);

      return s;
    }
  } // namespace musl
} // namespace mingw_thunk
