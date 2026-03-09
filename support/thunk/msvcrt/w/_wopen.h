#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace f
  {
    int win9x__wopen(const wchar_t *path, int flags, ...);
  } // namespace f
} // namespace mingw_thunk
