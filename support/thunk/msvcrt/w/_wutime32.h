#pragma once

#include <sys/utime.h>

namespace mingw_thunk
{
  namespace f
  {
    int win9x__wutime32(const wchar_t *filename, struct __utimbuf32 *times);
  } // namespace f
} // namespace mingw_thunk
