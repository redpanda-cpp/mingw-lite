#pragma once

#include <sys/stat.h>

namespace mingw_thunk
{
  namespace f
  {
    int win9x__wstat32(const wchar_t *path, struct _stat32 *buffer);
  } // namespace f
} // namespace mingw_thunk
