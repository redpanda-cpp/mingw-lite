#pragma once

#include <malloc.h>

namespace mingw_thunk
{
  namespace f
  {
    void *fallback__aligned_malloc(size_t size, size_t alignment);
  } // namespace f
} // namespace mingw_thunk
