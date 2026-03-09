#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace f
  {
    int win9x__wchmod(const wchar_t *filename, int pmode);
  } // namespace f
} // namespace mingw_thunk
