#pragma once

#include "../include/features.h"

namespace mingw_thunk
{
  namespace musl
  {
    struct FILE;

    hidden unsigned long long
    __intscan(FILE *f, unsigned base, int pok, unsigned long long lim);
  } // namespace musl
} // namespace mingw_thunk
