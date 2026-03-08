#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
#include <sys/utime.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _wutime32,
                 const wchar_t *filename,
                 struct __utimbuf32 *times)
  {
    if (internal::is_nt())
      return __ms__wutime32(filename, times);

    if (!filename) {
      _set_errno(EINVAL);
      return -1;
    }

    d::a_str a_path;
    if (!a_path.from_w(filename)) {
      _set_errno(ENOMEM);
      return -1;
    }

    return __ms__utime32(a_path.c_str(), times);
  }
} // namespace mingw_thunk
