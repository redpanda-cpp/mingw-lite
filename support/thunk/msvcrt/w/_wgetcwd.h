#pragma once

#include <stdlib.h>

namespace mingw_thunk
{
  namespace f
  {
    wchar_t *win9x__wgetcwd(wchar_t *buffer, int maxlen);
  } // namespace f
} // namespace mingw_thunk
