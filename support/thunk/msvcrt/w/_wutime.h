#pragma once

#include <sys/utime.h>

namespace mingw_thunk
{
  namespace f
  {
    int win9x__wutime(const wchar_t *filename, struct _utimbuf *times);
  } // namespace f
} // namespace mingw_thunk
