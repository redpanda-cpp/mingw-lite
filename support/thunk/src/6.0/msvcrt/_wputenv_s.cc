#include <thunk/_common.h>
#include <thunk/wntcrt/errno.h>

#include <errno.h>
#include <wchar.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(errno_t,
                     _wputenv_s,
                     const wchar_t *varname,
                     const wchar_t *value_string)
  {
    if (const auto pfn = try_get__wputenv_s())
      return pfn(varname, value_string);

    if (!varname || !value_string) {
      _set_errno(EINVAL);
      return EINVAL;
    }

    if (!SetEnvironmentVariableW(varname, value_string))
      return internal::dosmaperr();

    return 0;
  }
} // namespace mingw_thunk
