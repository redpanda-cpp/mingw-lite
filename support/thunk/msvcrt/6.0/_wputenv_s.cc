#include "_wputenv_s.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/string.h>

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
    __DISPATCH_THUNK_2(_wputenv_s,
                       const auto pfn = try_get__wputenv_s(),
                       pfn,
                       &f::fallback__wputenv_s);

    return dllimport__wputenv_s(varname, value_string);
  }

  namespace f
  {
    errno_t fallback__wputenv_s(const wchar_t *varname,
                                const wchar_t *value_string)
    {
      if (!varname || !value_string) {
        _set_errno(EINVAL);
        return EINVAL;
      }

      size_t name_len = wcslen(varname);
      size_t value_len = wcslen(value_string);

      d::w_str envstring;
      if (!envstring.resize(name_len + 1 + value_len)) {
        _set_errno(ENOMEM);
        return ENOMEM;
      }

      wmemcpy(envstring.data(), varname, name_len);
      envstring[name_len] = L'=';
      wmemcpy(envstring.data() + name_len + 1, value_string, value_len);

      int ret = _wputenv(envstring.c_str());
      if (ret == -1) {
        return errno;
      }

      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
