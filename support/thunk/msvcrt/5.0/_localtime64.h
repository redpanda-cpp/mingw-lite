#pragma once

#include <time.h>

namespace mingw_thunk
{
  namespace impl
  {
    struct tm *
    time32__localtime64(const __time64_t *sourceTime);
  } // namespace impl
} // namespace mingw_thunk
