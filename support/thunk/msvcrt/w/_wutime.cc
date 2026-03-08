#define _USE_32BIT_TIME_T

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
    if (internal::is_nt())
      return __ms__wutime32(filename, (__utimbuf32 *)times);

    if (!filename)
      return -1;

    d::a_str a_path;
    if (!a_path.from_w(filename)) {
      _set_errno(ENOMEM);
      return -1;
    }

    return __ms__utime32(a_path.c_str(), (__utimbuf32 *)times);
  }
} // namespace mingw_thunk
