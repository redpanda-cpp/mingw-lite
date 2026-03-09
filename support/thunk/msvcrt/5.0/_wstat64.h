#pragma once

#include <thunk/wntcrt/stat.h>

namespace mingw_thunk
{
  namespace f
  {
    int time32__wstat64(const wchar_t *path, struct _stat64 *buffer);
  } // namespace f
} // namespace mingw_thunk
