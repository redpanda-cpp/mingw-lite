#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace f
  {
    int win9x__wfindnext(intptr_t handle, struct _wfinddata32_t *fileinfo);
  } // namespace f
} // namespace mingw_thunk
