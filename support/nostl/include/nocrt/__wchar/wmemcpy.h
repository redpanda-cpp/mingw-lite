#pragma once

#include <stddef.h>

#include "../__config.h"

namespace NS_NOCRT
{
  inline wchar_t *wmemmove(wchar_t *dest, const wchar_t *src, size_t n)
  {
    if (dest < src) {
      for (size_t i = 0; i < n; ++i)
        dest[i] = src[i];
    } else {
      for (size_t i = n; i > 0; --i)
        dest[i - 1] = src[i - 1];
    }
    return dest;
  }

  inline wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t n)
  {
    return wmemmove(dest, src, n);
  }

} // namespace NS_NOCRT
