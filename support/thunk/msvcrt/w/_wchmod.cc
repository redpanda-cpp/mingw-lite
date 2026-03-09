#include "_wchmod.h"

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
    __DISPATCH_THUNK_2(_wchmod, i::is_nt(), &__ms__wchmod, &f::win9x__wchmod);

    return dllimport__wchmod(filename, pmode);
  }

  namespace f
  {
    int win9x__wchmod(const wchar_t *filename, int pmode)
    {
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
  } // namespace f
} // namespace mingw_thunk
