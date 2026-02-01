#include "_aligned_malloc.h"

#include <thunk/_common.h>

#include <errno.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 void *,
                 __cdecl,
                 _aligned_malloc,
                 size_t size,
                 size_t alignment)
  {
    if (const auto pfn = try_get__aligned_malloc())
      return pfn(size, alignment);

    return impl::fallback__aligned_malloc(size, alignment);
  }

  namespace impl
  {
    void *fallback__aligned_malloc(size_t size, size_t alignment)
    {
      bool is_power_of_2 = (alignment & (alignment - 1)) == 0;
      if (!is_power_of_2 || size == 0) {
        _set_errno(EINVAL);
        return nullptr;
      }

      void *allocation = ::malloc(size + alignment);
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
  } // namespace impl
} // namespace mingw_thunk
