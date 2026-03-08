#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, _wchmod, const wchar_t *filename, int pmode)
  {
    if (internal::is_nt())
      return __ms__wchmod(filename, pmode);

    if (!filename) {
      _set_errno(EINVAL);
      return -1;
    }

    d::a_str a_name;
    if (!a_name.from_w(filename)) {
      _set_errno(ENOMEM);
      return -1;
    }

    return __ms__chmod(a_name.c_str(), pmode);
  }
} // namespace mingw_thunk
