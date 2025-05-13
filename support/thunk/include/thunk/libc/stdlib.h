#pragma once

#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace mingw_thunk::internal
{
  inline int atoi(const char *nptr)
  {
    int result = 0;
    while (*nptr >= '0' && *nptr <= '9') {
      result = result * 10 + (*nptr - '0');
      nptr++;
    }
    return result;
  }

  template <typename T>
  void free(T *ptr) noexcept
  {
    if (ptr)
      HeapFree(GetProcessHeap(), 0, ptr);
  }

  template <typename T = void>
  T *malloc(size_t size) noexcept
  {
    return reinterpret_cast<T *>(HeapAlloc(GetProcessHeap(), 0, size));
  }

  template <typename T>
  T *realloc(T *ptr, size_t size) noexcept
  {
    return reinterpret_cast<T *>(HeapReAlloc(GetProcessHeap(), 0, ptr, size));
  }
} // namespace mingw_thunk::internal
