#pragma once

#include <stddef.h>
#include <stdint.h>

#include <heapapi.h>

namespace mingw_thunk
{
  namespace c
  {
    inline void *malloc(size_t size);
    inline void free(void *ptr);

    inline void *aligned_alloc(size_t alignment, size_t size)
    {
      bool is_power_of_2 = (alignment & (alignment - 1)) == 0;

      if (!is_power_of_2 || size % alignment != 0 || size == 0)
        return nullptr;

      // don't break default alignment
      static_assert(sizeof(void *) <= MEMORY_ALLOCATION_ALIGNMENT);
      if (alignment < MEMORY_ALLOCATION_ALIGNMENT)
        alignment = MEMORY_ALLOCATION_ALIGNMENT;

      void *allocation =
          HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size + alignment);
      if (!allocation)
        return nullptr;

      uintptr_t high_bits_mask = ~(alignment - 1);
      void *aligned = reinterpret_cast<void *>(
          (reinterpret_cast<uintptr_t>(allocation) + alignment) &
          high_bits_mask);

      // save the original allocation pointer exactly before the aligned pointer
      reinterpret_cast<void **>(aligned)[-1] = allocation;

      return aligned;
    }

    inline void *calloc(size_t num, size_t size)
    {
      return aligned_alloc(1, num * size);
    }

    inline void *malloc(size_t size)
    {
      return aligned_alloc(1, size);
    }

    inline void *realloc(void *ptr, size_t size)
    {
      if (!ptr)
        return malloc(size);
      if (size == 0) {
        free(ptr);
        return nullptr;
      }

      constexpr size_t alignment = MEMORY_ALLOCATION_ALIGNMENT;

      // safe. since we use default alignment,
      // the aligned pointer is always at (allocation + alignment).
      void *old_allocation = static_cast<void **>(ptr)[-1];
      void *new_allocation =
          HeapReAlloc(GetProcessHeap(), 0, old_allocation, size + alignment);
      if (!new_allocation)
        return nullptr;

      void *aligned = reinterpret_cast<void *>(
          (reinterpret_cast<uintptr_t>(new_allocation) + alignment));

      reinterpret_cast<void **>(aligned)[-1] = new_allocation;

      return aligned;
    }

    inline void free(void *ptr)
    {
      if (ptr) {
        void *allocation = static_cast<void **>(ptr)[-1];
        HeapFree(GetProcessHeap(), 0, allocation);
      }
    }
  } // namespace c
} // namespace mingw_thunk
