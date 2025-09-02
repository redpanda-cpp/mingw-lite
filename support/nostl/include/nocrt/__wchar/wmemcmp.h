#pragma once

#include <stddef.h>

#include "../__config.h"

namespace NS_NOCRT
{
  inline int wmemcmp(const wchar_t *s1, const wchar_t *s2, size_t n)
  {
    for (size_t i = 0; i < n; ++i) {
      if (s1[i] != s2[i])
        return s1[i] < s2[i] ? -1 : 1;
    }
    return 0;
  }

} // namespace NS_NOCRT
