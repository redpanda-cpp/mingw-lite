#include "_wfopen.h"
#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

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
    if (internal::is_nt())
      return __ms__wfopen(filename, mode);

    return impl::win9x__wfopen(filename, mode);
  }

  namespace impl
  {
    FILE *win9x__wfopen(const wchar_t *filename, const wchar_t *mode)
    {
      auto aname = internal::w2a(filename);
      auto amode = internal::w2a(mode);
      return __ms_fopen(aname.c_str(), amode.c_str());
    }
  } // namespace impl
} // namespace mingw_thunk
