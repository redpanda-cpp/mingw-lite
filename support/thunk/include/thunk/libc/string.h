#pragma once

#include <stddef.h>

namespace mingw_thunk::internal
{
  inline char *strchr(const char *str, int c) noexcept
  {
    while (*str) {
      if (*str == c) {
        return const_cast<char *>(str);
      }
      ++str;
    }
    return nullptr;
  }

  inline void *strcpy(char *dst, const char *src) noexcept
  {
    char *cur = dst;
    while (*src) {
      *cur++ = *src++;
    }
    *cur = 0;
    return dst;
  }

  inline char *stpncpy(char *dst, const char *src, size_t dsize) noexcept
  {
    char *dend = dst + dsize;
    while (dst < dend && *src)
      *dst++ = *src++;
    char *ret = dst;
    while (dst < dend)
      *dst++ = 0;
    return ret;
  }

  inline void *memcpy(void *dest, const void *src, size_t size) noexcept
  {
    char *dest_char = static_cast<char *>(dest);
    const char *src_char = static_cast<const char *>(src);
    while (size) {
      --size;
      *dest_char++ = *src_char++;
    }
    return dest;
  }
} // namespace mingw_thunk::internal
