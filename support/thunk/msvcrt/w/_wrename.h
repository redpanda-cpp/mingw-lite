#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace f
  {
    int win9x__wrename(const wchar_t *oldpath, const wchar_t *newpath);
  } // namespace f
} // namespace mingw_thunk
