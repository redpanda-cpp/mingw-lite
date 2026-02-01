#include "_gmtime64.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/time.h>

#include <time.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, struct tm *, __cdecl, _gmtime64, const __time64_t *sourceTime)
  {
    if (const auto pfn = try_get__gmtime64())
      return pfn(sourceTime);

    return impl::time32__gmtime64(sourceTime);
  }

  namespace impl
  {
    struct tm *time32__gmtime64(const __time64_t *sourceTime)
    {
      thread_local struct tm tm;
      internal::__secs_to_tm(*sourceTime, &tm);
      tm.tm_isdst = 0;
      return &tm;
    }
  } // namespace impl
} // namespace mingw_thunk
