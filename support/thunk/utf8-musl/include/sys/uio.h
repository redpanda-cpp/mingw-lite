#pragma once

#include <stddef.h>

namespace mingw_thunk
{
  namespace musl
  {
    struct iovec
    {
      void *iov_base;
      size_t iov_len;
    };
  } // namespace musl
} // namespace mingw_thunk
