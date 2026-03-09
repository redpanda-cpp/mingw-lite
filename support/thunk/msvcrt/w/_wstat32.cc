#include "_wstat32.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
#include <sys/stat.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _wstat32,
                 const wchar_t *path,
                 struct _stat32 *buffer)
  {
    __DISPATCH_THUNK_2(
        _wstat32, i::is_nt(), &__ms__wstat32, &f::win9x__wstat32);

    return dllimport__wstat32(path, buffer);
  }

  namespace f
  {
    int win9x__wstat32(const wchar_t *path, struct _stat32 *buffer)
    {
      if (!path) {
        _set_errno(EINVAL);
        return -1;
      }

      d::a_str a_path;
      if (!a_path.from_w(path)) {
        _set_errno(ENOMEM);
        return -1;
      }

      return __ms__stat32(a_path.c_str(), buffer);
    }
  } // namespace f
} // namespace mingw_thunk
