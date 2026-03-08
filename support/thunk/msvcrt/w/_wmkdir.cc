#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <direct.h>
#include <errno.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _wmkdir, const wchar_t *dirname)
  {
    if (internal::is_nt())
      return __ms__wmkdir(dirname);

    if (!dirname) {
      _set_errno(EINVAL);
      return -1;
    }

    d::a_str a_name;
    if (!a_name.from_w(dirname)) {
      _set_errno(ENOMEM);
      return -1;
    }

    return __ms__mkdir(a_name.c_str());
  }
} // namespace mingw_thunk
