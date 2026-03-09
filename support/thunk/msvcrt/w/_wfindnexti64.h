#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace f
  {
    int win9x__wfindnexti64(intptr_t handle,
                            struct _wfinddata32i64_t *fileinfo);
  } // namespace f
} // namespace mingw_thunk
