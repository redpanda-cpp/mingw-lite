#include "_localtime64.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/time.h>

#include <time.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 struct tm *,
                 __cdecl,
                 _localtime64,
                 const __time64_t *sourceTime)
  {
    if (const auto pfn = try_get__localtime64())
      return pfn(sourceTime);

    return impl::time32__localtime64(sourceTime);
  }

  namespace impl
  {
    struct tm *time32__localtime64(const __time64_t *sourceTime)
    {
      thread_local struct tm tm;

      if (*sourceTime <= 0x7FFFFFFF) {
        __time32_t time32 = *sourceTime;
        struct tm *tm32 = _localtime32(&time32);
        if (tm32) {
          tm = *tm32;
          return &tm;
        } else {
          return nullptr;
        }
      } else {
        // the epoch is 2038-01-19 03:14:07 UTC.
        // Microsoft doesn't support time after 2038-01-18 23:59:59 UTC.
        // the time zone range is -12:00 to +14:00.
        // use 1 day before the epoch should be safe to estimate the offset.
        __time32_t safe_test_time = 0x7FFFFFFF - 24 * 60 * 60;

        // for msvcrt without _localtime64, there is no _mkgmtime either.
        struct tm test_tm = *_gmtime32(&safe_test_time);
        __time32_t pseudo_time = _mktime32(&test_tm);

        // take UTC+8 for example:
        //   original time_t:        0
        //   converted struct tm:    0:00 UTC
        //   treat it as local time: 0:00 UTC+8
        //   converted time_t:       -8 * 60 * 60
        //   offset:                 8 * 60 * 60
        __time32_t offset = safe_test_time - pseudo_time;

        internal::__secs_to_tm(*sourceTime + offset, &tm);
        tm.tm_isdst = test_tm.tm_isdst;
        return &tm;
      }
    }
  } // namespace impl
} // namespace mingw_thunk
