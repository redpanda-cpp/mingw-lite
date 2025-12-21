#include <thunk/_common.h>

#include <time.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, char *, __cdecl, _ctime64, const __time64_t *sourceTime)
  {
    struct tm *tm = _localtime64(sourceTime);
    if (!tm)
      return nullptr;

    // “ASCII time”, no action required.
    return asctime(tm);
  };
} // namespace mingw_thunk
