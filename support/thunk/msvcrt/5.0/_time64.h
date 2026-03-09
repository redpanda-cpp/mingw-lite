#pragma once

#include <time.h>

namespace mingw_thunk
{
  namespace f
  {
    __time64_t time32__time64(__time64_t *destTime);
  }
} // namespace mingw_thunk
