#pragma once

#include <stddef.h>

#if __has_include(<new>)
  #include <new>
#else
inline void *operator new(size_t size, void *ptr) noexcept
{
  return ptr;
}
#endif
