#include "../include/stdlib.h"
#include "../include/wchar.h"

namespace mingw_thunk
{
  namespace musl
  {
    int wctomb(char *s, char32_t wc)
    {
      if (!s)
        return 0;
      return wcrtomb(s, wc, 0);
    }
  } // namespace musl
} // namespace mingw_thunk
