#include "../internal/stdio_impl.h"
#include <string.h>

namespace mingw_thunk
{
  namespace musl
  {
    size_t fread(void *destv, size_t size, size_t nmemb, FILE *f)
    {
      unsigned char *dest = (unsigned char *)destv;
      size_t len = size * nmemb, l = len, k;
      if (!size)
        nmemb = 0;

      FLOCK(f);

      f->mode |= f->mode - 1;

      if (f->rpos != f->rend) {
        size_t avail = f->rend - f->rpos;
        k = avail < l ? avail : l;
        memcpy(dest, f->rpos, k);
        f->rpos += k;
        dest += k;
        l -= k;
      }

      for (; l; l -= k, dest += k) {
        k = __toread(f) ? 0 : f->read(f, dest, l);
        if (!k) {
          FUNLOCK(f);
          return (len - l) / size;
        }
      }

      FUNLOCK(f);
      return nmemb;
    }
  } // namespace musl
} // namespace mingw_thunk
