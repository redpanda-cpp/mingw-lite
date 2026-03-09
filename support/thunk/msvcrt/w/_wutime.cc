#define _USE_32BIT_TIME_T

#include "_wutime.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
#include <sys/utime.h>

namespace mingw_thunk
{
  static_assert(sizeof(_utimbuf) == sizeof(__utimbuf32));

  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _wutime,
                 const wchar_t *filename,
                 struct _utimbuf *times)
  {
    __DISPATCH_THUNK_2(_wutime,
                       i::is_nt(),
                       (decltype(&_wutime))&__ms__wutime32,
                       &f::win9x__wutime);

    return dllimport__wutime(filename, times);
  }

  namespace f
  {
    int win9x__wutime(const wchar_t *filename, struct _utimbuf *times)
    {
      if (!filename) {
        _set_errno(EINVAL);
        return -1;
      }

      d::a_str a_path;
      if (!a_path.from_w(filename)) {
        _set_errno(ENOMEM);
        return -1;
      }

      return __ms__utime32(a_path.c_str(), (__utimbuf32 *)times);
    }
  } // namespace f
} // namespace mingw_thunk
