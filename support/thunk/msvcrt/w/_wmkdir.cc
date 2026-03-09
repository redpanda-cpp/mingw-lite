#include "_wmkdir.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <direct.h>
#include <errno.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, _wmkdir, const wchar_t *dirname)
  {
    __DISPATCH_THUNK_2(_wmkdir, i::is_nt(), &__ms__wmkdir, &f::win9x__wmkdir);

    return dllimport__wmkdir(dirname);
  }

  namespace f
  {
    int win9x__wmkdir(const wchar_t *dirname)
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

      return __ms__mkdir(a_name.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
