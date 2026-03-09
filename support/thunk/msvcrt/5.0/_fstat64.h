#pragma once

#include <thunk/wntcrt/stat.h>

namespace mingw_thunk
{
  namespace f
  {
    int time32__fstat64(int fd, struct _stat64 *buffer);
  } // namespace f
} // namespace mingw_thunk
