#include <iconv.h>

#include <stdlib.h>

namespace libiconv
{
  extern "C" int iconv_close(iconv_t cd)
  {
    if (cd == (iconv_t)-1)
      return 0;

    free(cd);
    return 0;
  }
} // namespace libiconv
