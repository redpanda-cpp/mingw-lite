#include "_time32.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/time.h>

#include <time.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, __time32_t, __cdecl, _time32, __time32_t *destTime)
  {
    __DISPATCH_THUNK_2(
        _time32, const auto pfn = try_get__time32(), pfn, &f::x64__time32);

    return dllimport__time32(destTime);
  };

  namespace f
  {
    __time32_t x64__time32(__time32_t *destTime)
    {
      __time64_t t;
      _time64(&t);
      if (destTime)
        *destTime = (__time32_t)t;
      return (__time32_t)t;
    }
  } // namespace f
} // namespace mingw_thunk
