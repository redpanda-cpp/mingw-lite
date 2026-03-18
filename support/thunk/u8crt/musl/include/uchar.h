#pragma once

#include <stddef.h>
#include <stdint.h>

#define __NEED_mbstate_t
#include "bits/alltypes.h"

namespace mingw_thunk
{
  namespace musl
  {
    size_t mbrtoc16(char16_t *pwc, const char *s, size_t n, mbstate_t *ps);
    size_t mbrtoc16(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps);
  } // namespace musl
} // namespace mingw_thunk
