#pragma once

#include <stdio.h>

namespace mingw_thunk
{
  namespace f
  {
    FILE *win9x__wfopen(const wchar_t *filename, const wchar_t *mode);
  } // namespace f
} // namespace mingw_thunk
