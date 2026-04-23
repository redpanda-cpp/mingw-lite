#pragma once

#include <time.h>

namespace mingw_thunk
{
  namespace f
  {
    __time32_t x64__time32(__time32_t *destTime);
  }
} // namespace mingw_thunk
