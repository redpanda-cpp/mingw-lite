#pragma once

#include <sys/stat.h>

namespace mingw_thunk
{
  namespace impl
  {
    int time32__wstat64(const wchar_t *path, struct _stat64 *buffer);
  } // namespace impl
} // namespace mingw_thunk
