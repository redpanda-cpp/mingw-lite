#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace f
  {
    int time32__wfindnext64(intptr_t handle, struct _wfinddata64_t *fileinfo);
  } // namespace f
} // namespace mingw_thunk
