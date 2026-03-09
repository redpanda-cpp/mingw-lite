#pragma once

#include <time.h>

namespace mingw_thunk
{
  namespace f
  {
    struct tm *time32__localtime64(const __time64_t *sourceTime);
  } // namespace f
} // namespace mingw_thunk
