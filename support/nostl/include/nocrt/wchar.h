#pragma once

#include <wchar.h>

#include "__config.h"

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

  inline int wmemcmp(const wchar_t *s1, const wchar_t *s2, size_t n)
  {
    for (size_t i = 0; i < n; ++i) {
      if (s1[i] != s2[i])
        return s1[i] < s2[i] ? -1 : 1;
    }
    return 0;
  }

  inline size_t wcslen(const wchar_t *s)
  {
    size_t len = 0;
    while (s[len] != 0)
      ++len;
    return len;
  }
} // namespace NS_NOCRT
