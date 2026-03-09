#pragma once

#include <thunk/wntcrt/stat.h>

namespace mingw_thunk
{
  namespace f
  {
    int time32__stat64(const char *path, struct _stat64 *buffer);
  }
} // namespace mingw_thunk
