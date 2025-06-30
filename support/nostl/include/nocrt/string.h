#pragma once

#include <stddef.h>

#include "__config.h"

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

  inline int memcmp(const void *s1, const void *s2, size_t n)
  {
    const unsigned char *s1_ = static_cast<const unsigned char *>(s1);
    const unsigned char *s2_ = static_cast<const unsigned char *>(s2);
    for (size_t i = 0; i < n; ++i) {
      if (s1_[i] != s2_[i])
        return s1_[i] < s2_[i] ? -1 : 1;
    }
    return 0;
  }

  inline size_t strlen(const char *s)
  {
    size_t len = 0;
    while (s[len] != 0)
      ++len;
    return len;
  }
} // namespace NS_NOCRT
