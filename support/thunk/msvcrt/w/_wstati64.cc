#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
#include <sys/stat.h>

#undef _wstati64

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _wstati64,
                 const wchar_t *path,
                 struct _stat32i64 *buffer)
  {
    if (internal::is_nt())
      return __ms__wstat32i64(path, buffer);

    if (!path) {
      _set_errno(EINVAL);
      return -1;
    }

    d::a_str a_path;
    if (!a_path.from_w(path)) {
      _set_errno(ENOMEM);
      return -1;
    }

    return __ms__stat32i64(a_path.c_str(), buffer);
  }
} // namespace mingw_thunk
