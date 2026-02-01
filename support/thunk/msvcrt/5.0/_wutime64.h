#pragma once

#include <sys/utime.h>

namespace mingw_thunk
{
  namespace impl
  {
    int time32__wutime64(const wchar_t *filename, struct __utimbuf64 *times);
  } // namespace impl
} // namespace mingw_thunk
