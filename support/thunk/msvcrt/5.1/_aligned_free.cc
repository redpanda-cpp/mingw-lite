#include "_aligned_free.h"

#include <thunk/_common.h>

#include <malloc.h>
#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, void, __cdecl, _aligned_free, void *memblock)
  {
    if (const auto pfn = try_get__aligned_free())
      return pfn(memblock);

    impl::fallback__aligned_free(memblock);
  }

  namespace impl
  {
    void fallback__aligned_free(void *memblock)
    {
      if (memblock) {
        void *allocation = static_cast<void **>(memblock)[-1];
        ::free(allocation);
      }
    }
  } // namespace impl
} // namespace mingw_thunk
