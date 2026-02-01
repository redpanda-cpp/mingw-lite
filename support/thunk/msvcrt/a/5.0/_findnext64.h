#pragma once

#include <io.h>

namespace mingw_thunk
{
  namespace impl
  {
    int time32__findnext64(intptr_t handle, struct __finddata64_t *fileinfo);
  } // namespace impl
} // namespace mingw_thunk
