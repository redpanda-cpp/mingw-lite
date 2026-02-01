#pragma once

#include <malloc.h>

namespace mingw_thunk
{
  namespace impl
  {
    void fallback__aligned_free(void *memblock);
  } // namespace impl
} // namespace mingw_thunk
