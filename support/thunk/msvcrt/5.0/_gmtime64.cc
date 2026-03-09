#include "_gmtime64.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/time.h>

#include <time.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, struct tm *, __cdecl, _gmtime64, const __time64_t *sourceTime)
  {
    __DISPATCH_THUNK_2(_gmtime64,
                       const auto pfn = try_get__gmtime64(),
                       pfn,
                       &f::time32__gmtime64);

    return dllimport__gmtime64(sourceTime);
  }

  namespace f
  {
    struct tm *time32__gmtime64(const __time64_t *sourceTime)
    {
      thread_local struct tm tm;
      internal::__secs_to_tm(*sourceTime, &tm);
      tm.tm_isdst = 0;
      return &tm;
    }
  } // namespace f
} // namespace mingw_thunk
