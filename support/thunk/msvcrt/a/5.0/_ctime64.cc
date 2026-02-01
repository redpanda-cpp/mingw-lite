#include <thunk/_common.h>
#include <thunk/wntcrt/time.h>

#include <time.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, char *, __cdecl, _ctime64, const __time64_t *sourceTime)
  {
    if (const auto pfn = try_get__ctime64())
      return pfn(sourceTime);

    struct tm *tm = _localtime64(sourceTime);
    if (!tm)
      return nullptr;
    return asctime(tm);
  };
} // namespace mingw_thunk
