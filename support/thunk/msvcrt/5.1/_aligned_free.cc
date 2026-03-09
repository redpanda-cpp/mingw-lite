#include "_aligned_free.h"

#include <thunk/_common.h>

#include <malloc.h>
#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, void, __cdecl, _aligned_free, void *memblock)
  {
    __DISPATCH_THUNK_2(_aligned_free,
                       const auto pfn = try_get__aligned_free(),
                       pfn,
                       &f::fallback__aligned_free);

    return dllimport__aligned_free(memblock);
  }

  namespace f
  {
    void fallback__aligned_free(void *memblock)
    {
      if (memblock) {
        void *allocation = static_cast<void **>(memblock)[-1];
        ::free(allocation);
      }
    }
  } // namespace f
} // namespace mingw_thunk
