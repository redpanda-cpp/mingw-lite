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
} // namespace mingw_thunk::internal
