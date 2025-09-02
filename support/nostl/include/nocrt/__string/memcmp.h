#pragma once

#include <stddef.h>

#include "../__config.h"

namespace NS_NOCRT
{
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

} // namespace NS_NOCRT
