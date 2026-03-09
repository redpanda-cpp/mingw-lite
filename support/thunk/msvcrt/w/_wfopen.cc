#include "_wfopen.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
#include <stdio.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 FILE *,
                 __cdecl,
                 _wfopen,
                 const wchar_t *filename,
                 const wchar_t *mode)
  {
    __DISPATCH_THUNK_2(_wfopen, i::is_nt(), &__ms__wfopen, &f::win9x__wfopen);

    return dllimport__wfopen(filename, mode);
  }

  namespace f
  {
    FILE *win9x__wfopen(const wchar_t *filename, const wchar_t *mode)
    {
      if (!filename || !mode) {
        _set_errno(EINVAL);
        return nullptr;
      }

      d::a_str a_name;
      if (!a_name.from_w(filename)) {
        _set_errno(ENOMEM);
        return nullptr;
      }

      d::a_str a_mode;
      if (!a_mode.from_w(mode)) {
        _set_errno(ENOMEM);
        return nullptr;
      }

      return __ms_fopen(a_name.c_str(), a_mode.c_str());
    }
  } // namespace f
} // namespace mingw_thunk
