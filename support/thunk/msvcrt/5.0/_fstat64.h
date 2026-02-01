#pragma once

#include <io.h>
#include <sys/stat.h>
#include <sys/utime.h>

namespace mingw_thunk
{
  namespace impl
  {
    int time32__fstat64(int fd, struct _stat64 *buffer);
  } // namespace impl
} // namespace mingw_thunk
