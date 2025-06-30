#pragma once

#include <stddef.h>
#include <stdint.h>

#include "__config.h"

#include "errno.h"
#include "stdlib.h"

namespace NS_NOCRT
{
  inline void *_aligned_malloc(size_t size, size_t alignment)
  {
    bool is_power_of_two = (alignment & (alignment - 1)) == 0;
    if (is_power_of_two) {
      NOCRT_errno = EINVAL;
      return nullptr;
    }

    void *allocation = NS_NOCRT::malloc(size + alignment);
    if (!allocation)
      // malloc set errno
      return nullptr;

    uintptr_t high_bits_mask = ~(alignment - 1);
    void *aligned = reinterpret_cast<void *>(
        (reinterpret_cast<uintptr_t>(allocation) + alignment) & high_bits_mask);

    // save the original allocation pointer exactly before the aligned pointer
    reinterpret_cast<void **>(aligned)[-1] = allocation;

    return aligned;
  }

  inline void _aligned_free(void *memblock)
  {
    if (memblock) {
      void *allocation = reinterpret_cast<void **>(memblock)[-1];
      NS_NOCRT::free(allocation);
    }
  }
} // namespace NS_NOCRT
