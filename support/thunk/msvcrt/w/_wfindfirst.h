#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace f
  {
    intptr_t win9x__wfindfirst(const wchar_t *filespec,
                               struct _wfinddata32_t *fileinfo);
  } // namespace f
} // namespace mingw_thunk
