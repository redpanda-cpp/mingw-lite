#pragma once

#include <stddef.h>

namespace mingw_thunk::internal
{
  inline size_t wcslen(const wchar_t *str) noexcept
  {
    size_t len = 0;
    while (*str++)
      ++len;
    return len;
  }

  inline size_t wcsnlen(const wchar_t *str, size_t maxlen) noexcept
  {
    size_t len = 0;
    while (len < maxlen && *str++)
      ++len;
    return len;
  }

  inline wchar_t *
  wcspncpy(wchar_t *dst, const wchar_t *src, size_t dsize) noexcept
  {
    wchar_t *dend = dst + dsize;
    while (dst < dend && *src)
      *dst++ = *src++;
    wchar_t *ret = dst;
    while (dst < dend)
      *dst++ = 0;
    return ret;
  }

  inline wchar_t *wcschr(const wchar_t *wcs, wchar_t wc) noexcept
  {
    while (*wcs) {
      if (*wcs == wc)
        return const_cast<wchar_t *>(wcs);
      ++wcs;
    }
    return nullptr;
  }
} // namespace mingw_thunk::internal
