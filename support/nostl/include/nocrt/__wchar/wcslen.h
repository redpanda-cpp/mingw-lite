#pragma once

#include <stddef.h>

#include "../__config.h"

namespace NS_NOCRT
{
  inline size_t wcslen(const wchar_t *s)
  {
    size_t len = 0;
    while (s[len] != 0)
      ++len;
    return len;
  }

  inline size_t wcsnlen(const wchar_t *s, size_t maxlen)
  {
    size_t len = 0;
    while (len < maxlen && s[len] != 0)
      ++len;
    return len;
  }

} // namespace NS_NOCRT
