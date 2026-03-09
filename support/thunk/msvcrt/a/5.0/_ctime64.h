#pragma once

#include <time.h>

namespace mingw_thunk
{
  namespace f
  {
    char *time32__ctime64(const __time64_t *sourceTime);
  } // namespace f
} // namespace mingw_thunk
