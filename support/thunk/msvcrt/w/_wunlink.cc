#include "_wunlink.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _wunlink, const wchar_t *path)
  {
    __DISPATCH_THUNK_2(
        _wunlink, i::is_nt(), &__ms__wunlink, &f::win9x__wunlink);

    return dllimport__wunlink(path);
  }

  namespace f
  {
    int win9x__wunlink(const wchar_t *path)
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

      return __ms__unlink(a_path.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
