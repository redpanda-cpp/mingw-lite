#pragma once

#include <stddef.h>

#include "../__config.h"

namespace NS_NOCRT
{
  inline wchar_t *wcschr(const wchar_t *wcs, wchar_t wc)
  {
    while (*wcs) {
      if (*wcs == wc)
        return const_cast<wchar_t *>(wcs);
      ++wcs;
    }
    return nullptr;
  }
} // namespace NS_NOCRT
