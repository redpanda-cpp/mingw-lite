#include <thunk/_common.h>
#include <thunk/wntcrt/time.h>

#include <time.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(struct tm *, _gmtime64, const __time64_t *sourceTime)
  {
    if (const auto pfn = try_get__gmtime64())
      return pfn(sourceTime);

    thread_local struct tm tm;
    internal::__secs_to_tm(*sourceTime, &tm);
    tm.tm_isdst = 0;
    return &tm;
  }
} // namespace mingw_thunk
