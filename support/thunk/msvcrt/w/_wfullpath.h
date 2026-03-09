#pragma once

#include <stdlib.h>

namespace mingw_thunk
{
  namespace f
  {
    wchar_t *win9x__wfullpath(wchar_t *absPath,
                              const wchar_t *relPath,
                              size_t maxLength);
  } // namespace f
} // namespace mingw_thunk
