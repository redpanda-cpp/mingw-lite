#include "_wchdir.h"

#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <direct.h>
#include <errno.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _wchdir, const wchar_t *dirname)
  {
    __DISPATCH_THUNK_2(_wchdir, i::is_nt(), &__ms__wchdir, &f::win9x__wchdir);

    return dllimport__wchdir(dirname);
  }

  namespace f
  {
    int win9x__wchdir(const wchar_t *dirname)
    {
      if (!dirname) {
        _set_errno(EINVAL);
        return -1;
      }

      d::a_str a_name;
      if (!a_name.from_w(dirname)) {
        _set_errno(ENOMEM);
        return -1;
      }

      return __ms__chdir(a_name.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
