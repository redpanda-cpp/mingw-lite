#pragma once

#include <sys/stat.h>

namespace mingw_thunk
{
  namespace impl
  {
    int time32__stat64(const char *path, struct __stat64 *buffer);
  }
} // namespace mingw_thunk
