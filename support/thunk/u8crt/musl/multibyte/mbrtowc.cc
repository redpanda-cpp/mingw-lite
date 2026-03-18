#include "../include/wchar.h"
#include "../internal/locale_impl.h"
#include "internal.h"

#include <errno.h>
#include <stddef.h>

namespace mingw_thunk
{
  namespace musl
  {
    size_t mbrtowc(char32_t *wc, const char *src, size_t n, mbstate_t *st)
    {
      static unsigned internal_state;
      unsigned c;
      const unsigned char *s = (const unsigned char *)src;
      const size_t N = n;
      char32_t dummy;

      if (!st)
        st = (mbstate_t *)&internal_state;
      c = *(unsigned *)st;

      if (!s) {
        if (c)
          goto ilseq;
        return 0;
      } else if (!wc)
        wc = &dummy;

      if (!n)
        return -2;
      if (!c) {
        if (*s < 0x80)
          return !!(*wc = *s);
        if (MB_CUR_MAX == 1)
          return (*wc = CODEUNIT(*s)), 1;
        if (*s - SA > SB - SA)
          goto ilseq;
        c = bittab[*s++ - SA];
        n--;
      }

      if (n) {
        if (OOB(c, *s))
          goto ilseq;
      loop:
        c = c << 6 | *s++ - 0x80;
        n--;
        if (!(c & (1U << 31))) {
          *(unsigned *)st = 0;
          *wc = c;
          return N - n;
        }
        if (n) {
          if (*s - 0x80u >= 0x40)
            goto ilseq;
          goto loop;
        }
      }

      *(unsigned *)st = c;
      return -2;
    ilseq:
      *(unsigned *)st = 0;
      errno = EILSEQ;
      return -1;
    }
  } // namespace musl
} // namespace mingw_thunk
