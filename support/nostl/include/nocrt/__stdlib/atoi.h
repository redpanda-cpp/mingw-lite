#pragma once

#include "../__config.h"

namespace NS_NOCRT
{
  inline int atoi(const char *nptr)
  {
    int result = 0;
    while (*nptr >= '0' && *nptr <= '9') {
      result = result * 10 + (*nptr - '0');
      nptr++;
    }
    return result;
  }
} // namespace NS_NOCRT
