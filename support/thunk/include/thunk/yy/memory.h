#pragma once

#include <thunk/libc/stdlib.h>

namespace std
{
  struct nothrow_t;
}

namespace mingw_thunk::internal
{
  template <typename T, typename... Args>
  T *New(Args &&...args) noexcept
  {
    T *ptr = static_cast<T *>(malloc(sizeof(T)));
    if (ptr)
      new (ptr) T(static_cast<Args &&>(args)...);
    return ptr;
  }

  template <typename T>
  void Delete(T *ptr) noexcept
  {
    if (ptr) {
      ptr->~T();
      free(ptr);
    }
  }

  struct CppAlloc
  {
    void *operator new(size_t count) noexcept { return malloc(count); }
    void *operator new[](size_t count) noexcept { return malloc(count); }
    void operator delete(void *ptr) noexcept { free(ptr); }
    void operator delete[](void *ptr) noexcept { free(ptr); }

    void *operator new(size_t count, const std::nothrow_t &) noexcept
    {
      return malloc(count);
    }
    void *operator new[](size_t count, const std::nothrow_t &) noexcept
    {
      return malloc(count);
    }
    void operator delete(void *ptr, const std::nothrow_t &) noexcept
    {
      free(ptr);
    }
    void operator delete[](void *ptr, const std::nothrow_t &) noexcept
    {
      free(ptr);
    }

    void *operator new(size_t count, void *ptr) noexcept { return ptr; }
    void *operator new[](size_t count, void *ptr) noexcept { return ptr; }
    void operator delete(void *ptr, void *) noexcept {}
    void operator delete[](void *ptr, void *) noexcept {}
  };
} // namespace mingw_thunk::internal
