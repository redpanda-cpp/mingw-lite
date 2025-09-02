#include <thunk/_common.h>

#include <malloc.h>
#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(void, _aligned_free, void *memblock)
  {
    if (const auto pfn = try_get__aligned_free())
      return pfn(memblock);

    if (memblock) {
      void *allocation = static_cast<void **>(memblock)[-1];
      ::free(allocation);
    }
  }
} // namespace mingw_thunk
