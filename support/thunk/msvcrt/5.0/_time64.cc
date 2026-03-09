#include "_time64.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/time.h>

#include <time.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, __time64_t, __cdecl, _time64, __time64_t *destTime)
  {
    __DISPATCH_THUNK_2(
        _time64, const auto pfn = try_get__time64(), pfn, &f::time32__time64);

    return dllimport__time64(destTime);
  };

  namespace f
  {
    __time64_t time32__time64(__time64_t *destTime)
    {
      FILETIME ft;
      GetSystemTimeAsFileTime(&ft);
      __time64_t ct = internal::c_time64_from_filetime(ft);
      if (destTime)
        *destTime = ct;
      return ct;
    }
  } // namespace f
} // namespace mingw_thunk
