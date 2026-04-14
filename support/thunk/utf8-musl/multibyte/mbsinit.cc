#include "../include/wchar.h"

namespace mingw_thunk
{
  namespace musl
  {
    int mbsinit(const mbstate_t *st)
    {
      return !st || !*(unsigned *)st;
    }
  } // namespace musl
} // namespace mingw_thunk
