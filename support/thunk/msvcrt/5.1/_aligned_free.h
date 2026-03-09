#pragma once

#include <malloc.h>

namespace mingw_thunk
{
  namespace f
  {
    void fallback__aligned_free(void *memblock);
  } // namespace f
} // namespace mingw_thunk
