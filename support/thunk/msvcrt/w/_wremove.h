#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace f
  {
    int win9x__wremove(const wchar_t *path);
  } // namespace f
} // namespace mingw_thunk
