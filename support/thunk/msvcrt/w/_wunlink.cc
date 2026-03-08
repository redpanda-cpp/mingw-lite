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
    if (internal::is_nt())
      return __ms__wunlink(path);

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
} // namespace mingw_thunk
