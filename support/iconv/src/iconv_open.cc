#include <iconv.h>

#include <errno.h>
#include <stdlib.h>

#include "encoding.h"

namespace libiconv
{
  extern "C" iconv_t iconv_open(const char *tocode, const char *fromcode)
  {
    if (!tocode || !fromcode) {
      errno = EINVAL;
      return (iconv_t)-1;
    }

    resolve_result to_res = resolve_encoding(tocode);
    if (to_res.page == unsigned(-1)) {
      errno = EINVAL;
      return (iconv_t)-1;
    }

    resolve_result from_res = resolve_encoding(fromcode);
    if (from_res.page == unsigned(-1)) {
      errno = EINVAL;
      return (iconv_t)-1;
    }

    auto cd = (iconv_descriptor *)calloc(1, sizeof(iconv_descriptor));
    if (!cd) {
      errno = ENOMEM;
      return (iconv_t)-1;
    }

    cd->from_page = from_res.page;
    cd->to_page = to_res.page;

    error_mode mode;
    if (to_res.mode == error_mode::ignore ||
        from_res.mode == error_mode::ignore)
      mode = error_mode::ignore;
    else if (to_res.mode == error_mode::replace ||
             from_res.mode == error_mode::replace)
      mode = error_mode::replace;
    else
      mode = error_mode::strict;
    cd->mode = mode;

    return cd;
  }
} // namespace libiconv
