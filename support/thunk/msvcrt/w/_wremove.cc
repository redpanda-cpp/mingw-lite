#include "_wremove.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _wremove, const wchar_t *path)
  {
    __DISPATCH_THUNK_2(
        _wremove, i::is_nt(), &__ms__wremove, &f::win9x__wremove);

    return dllimport__wremove(path);
  }

  namespace f
  {
    int win9x__wremove(const wchar_t *path)
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

      return __ms_remove(a_path.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
