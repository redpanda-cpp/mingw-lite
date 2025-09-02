#pragma once

#include <stddef.h>

#include "../__config.h"

namespace NS_NOCRT
{
  inline char *strchr(const char *str, int c)
  {
    while (*str) {
      if (*str == c) {
        return const_cast<char *>(str);
      }
      ++str;
    }
    return nullptr;
  }

} // namespace NS_NOCRT
