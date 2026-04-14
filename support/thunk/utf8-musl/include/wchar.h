#pragma once

#include <stddef.h>
#include <stdint.h>

#define __NEED_mbstate_t
#include "bits/alltypes.h"

namespace mingw_thunk
{
  namespace musl
  {
    int mbsinit(const mbstate_t *);
    size_t mbrtowc(char32_t *pwc, const char *s, size_t n, mbstate_t *ps);
    size_t wcrtomb(char *s, char32_t wc, mbstate_t *st);
  } // namespace musl
} // namespace mingw_thunk
