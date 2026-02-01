#pragma once

#include <malloc.h>

namespace mingw_thunk
{
  namespace impl
  {
    void *fallback__aligned_malloc(size_t size, size_t alignment);
  } // namespace impl
} // namespace mingw_thunk
