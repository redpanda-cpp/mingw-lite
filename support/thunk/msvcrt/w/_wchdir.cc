#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <direct.h>
#include <errno.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _wchdir, const wchar_t *dirname)
  {
    if (internal::is_nt())
      return __ms__wchdir(dirname);

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
} // namespace mingw_thunk
