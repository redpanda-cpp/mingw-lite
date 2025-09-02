#pragma once

#include <stddef.h>

#include "../__config.h"

namespace NS_NOCRT
{
  inline void *memmove(void *dest, const void *src, size_t n)
  {
    char *dest_ = static_cast<char *>(dest);
    const char *src_ = static_cast<const char *>(src);
    if (dest < src) {
      for (size_t i = 0; i < n; ++i)
        dest_[i] = src_[i];
    } else {
      for (size_t i = n; i > 0; --i)
        dest_[i - 1] = src_[i - 1];
    }
    return dest;
  }

  inline void *memcpy(void *dest, const void *src, size_t n)
  {
    return memmove(dest, src, n);
  }

} // namespace NS_NOCRT
