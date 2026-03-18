#include "_ctime64.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/time.h>

#include <time.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, char *, __cdecl, _ctime64, const __time64_t *sourceTime)
  {
    __DISPATCH_THUNK_2(_ctime64,
                       const auto pfn = try_get__ctime64(),
                       pfn,
                       &f::time32__ctime64);

    return dllimport__ctime64(sourceTime);
  }

  namespace f
  {
    char *time32__ctime64(const __time64_t *sourceTime)
    {
      struct tm *tm = _localtime64(sourceTime);
      if (!tm)
        return nullptr;

      // “ASCII time”, not affected by UTF-8.
      return asctime(tm);
    }
  } // namespace f
} // namespace mingw_thunk
