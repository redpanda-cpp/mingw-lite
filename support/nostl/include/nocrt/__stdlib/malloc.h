#pragma once

#include <heapapi.h>

#include "../errno.h"

namespace NS_NOCRT
{
  inline HANDLE process_heap()
  {
    static HANDLE heap = GetProcessHeap();
    return heap;
  }

  inline void *malloc(size_t size)
  {
    void *allocation = HeapAlloc(process_heap(), 0, size);
    if (!allocation) {
      NOCRT_errno = ENOMEM;
      return nullptr;
    }
    return allocation;
  }

  inline void *realloc(void *ptr, size_t size)
  {
    void *allocation = nullptr;
    if (ptr)
      allocation = HeapReAlloc(process_heap(), 0, ptr, size);
    else
      allocation = HeapAlloc(process_heap(), 0, size);
    if (!allocation) {
      NOCRT_errno = ENOMEM;
      return nullptr;
    }
    return allocation;
  }

  inline void free(void *ptr)
  {
    if (ptr)
      HeapFree(process_heap(), 0, ptr);
  }
} // namespace NS_NOCRT
