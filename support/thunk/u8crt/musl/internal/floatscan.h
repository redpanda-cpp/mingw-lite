#pragma once

#include "../include/features.h"

namespace mingw_thunk
{
  namespace musl
  {
    struct FILE;

    hidden long double __floatscan(FILE *f, int prec, int pok);
  } // namespace musl
} // namespace mingw_thunk
