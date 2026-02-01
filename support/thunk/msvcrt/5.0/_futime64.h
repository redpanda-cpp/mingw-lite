#pragma once

#include <io.h>
#include <sys/stat.h>
#include <sys/utime.h>

namespace mingw_thunk
{
  namespace impl
  {
    int time32__futime64(int fd, struct __utimbuf64 *filetime);
  } // namespace impl
} // namespace mingw_thunk
