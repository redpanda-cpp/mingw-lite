#pragma once

#include <stdio.h>

namespace mingw_thunk
{
  namespace impl
  {
    FILE *
    win9x__wfopen(const wchar_t *filename, const wchar_t *mode);
  } // namespace impl
} // namespace mingw_thunk
