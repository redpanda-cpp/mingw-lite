#pragma once

#include <io.h>
#include <sys/stat.h>
#include <sys/utime.h>

namespace mingw_thunk
{
  namespace f
  {
    int time32__futime64(int fd, struct __utimbuf64 *filetime);
  } // namespace f
} // namespace mingw_thunk
