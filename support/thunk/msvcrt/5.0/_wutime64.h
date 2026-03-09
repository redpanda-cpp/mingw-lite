#pragma once

#include <sys/utime.h>

namespace mingw_thunk
{
  namespace f
  {
    int time32__wutime64(const wchar_t *filename, struct __utimbuf64 *times);
  } // namespace f
} // namespace mingw_thunk
