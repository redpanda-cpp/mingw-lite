#include <thunk/_common.h>

#include <time.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      api_ms_win_crt_time_l1_1_0, 0, char *, __cdecl, _ctime64, const __time64_t *sourceTime)
  {
    struct tm *tm = _localtime64(sourceTime);
    if (!tm)
      return nullptr;

    // “ASCII time”, no action required.
    return asctime(tm);
  };
} // namespace mingw_thunk
