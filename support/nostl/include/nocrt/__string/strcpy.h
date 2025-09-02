#pragma once

#include <stddef.h>

#include "../__config.h"

namespace NS_NOCRT
{
  inline void *strcpy(char *dst, const char *src)
  {
    char *cur = dst;
    while (*src) {
      *cur++ = *src++;
    }
    *cur = 0;
    return dst;
  }

  inline char *stpncpy(char *dst, const char *src, size_t dsize)
  {
    char *dend = dst + dsize;
    while (dst < dend && *src)
      *dst++ = *src++;
    char *ret = dst;
    while (dst < dend)
      *dst++ = 0;
    return ret;
  }

} // namespace NS_NOCRT
