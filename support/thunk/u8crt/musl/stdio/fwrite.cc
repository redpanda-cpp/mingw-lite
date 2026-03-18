#include "../internal/stdio_impl.h"

#include <string.h>

namespace mingw_thunk
{
  namespace musl
  {
    size_t __fwritex(const unsigned char *s, size_t l, FILE *f)
    {
      size_t i = 0;

      if (!f->wend && __towrite(f))
        return 0;

      if (l > f->wend - f->wpos)
        return f->write(f, s, l);

      if (f->lbf >= 0) {
        /* Match /^(.*\n|)/ */
        for (i = l; i && s[i - 1] != '\n'; i--)
          ;
        if (i) {
          size_t n = f->write(f, s, i);
          if (n < i)
            return n;
          s += i;
          l -= i;
        }
      }

      memcpy(f->wpos, s, l);
      f->wpos += l;
      return l + i;
    }

    size_t fwrite(const void *src, size_t size, size_t nmemb, FILE *f)
    {
      size_t k, l = size * nmemb;
      if (!size)
        nmemb = 0;
      FLOCK(f);
      k = __fwritex((const unsigned char *)src, l, f);
      FUNLOCK(f);
      return k == l ? nmemb : k / size;
    }
  } // namespace musl
} // namespace mingw_thunk
