#include "_wputenv_s.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/errno.h>

#include <errno.h>
#include <wchar.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 errno_t,
                 __cdecl,
                 _wputenv_s,
                 const wchar_t *varname,
                 const wchar_t *value_string)
  {
    if (const auto pfn = try_get__wputenv_s())
      return pfn(varname, value_string);

    return impl::fallback__wputenv_s(varname, value_string);
  }

  namespace impl
  {
    errno_t fallback__wputenv_s(const wchar_t *varname,
                                const wchar_t *value_string)
    {
      if (!varname || !value_string) {
        _set_errno(EINVAL);
        return EINVAL;
      }

      if (!SetEnvironmentVariableW(varname, value_string))
        return internal::dosmaperr();

      return 0;
    }
  } // namespace impl
} // namespace mingw_thunk
