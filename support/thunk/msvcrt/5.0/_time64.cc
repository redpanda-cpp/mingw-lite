#include <thunk/_common.h>
#include <thunk/wntcrt/time.h>

#include <time.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, __time64_t, __cdecl, _time64, __time64_t *destTime)
  {
    if (const auto pfn = try_get__time64())
      return pfn(destTime);

    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    __time64_t ct = internal::c_time64_from_filetime(ft);
    if (destTime)
      *destTime = ct;
    return ct;
  };
} // namespace mingw_thunk
