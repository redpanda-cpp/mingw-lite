#include "../include/uchar.h"
#include "../include/wchar.h"

namespace mingw_thunk
{
  namespace musl
  {
    size_t mbrtoc16(char16_t *pc16, const char *s, size_t n, mbstate_t *ps)
    {
      static unsigned internal_state;
      if (!ps)
        ps = (mbstate_t *)&internal_state;
      unsigned *pending = (unsigned *)ps;

      if (!s)
        return mbrtoc16((char16_t *)0, "", 1, ps);

      /* mbrtowc states for partial UTF-8 characters have the high bit set;
       * we use nonzero states without high bit for pending surrogates. */
      if ((int)*pending > 0) {
        if (pc16)
          *pc16 = *pending;
        *pending = 0;
        return -3;
      }

      char32_t wc;
      size_t ret = mbrtowc(&wc, s, n, ps);
      if (ret <= 4) {
        if (wc >= 0x10000) {
          *pending = (wc & 0x3ff) + 0xdc00;
          wc = 0xd7c0 + (wc >> 10);
        }
        if (pc16)
          *pc16 = wc;
      }
      return ret;
    }

    size_t mbrtoc16(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps)
    {
      return mbrtoc16((char16_t *)pwc, s, n, ps);
    }
  } // namespace musl
} // namespace mingw_thunk
