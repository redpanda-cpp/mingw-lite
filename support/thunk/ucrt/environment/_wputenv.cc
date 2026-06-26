#include "__p__environ.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_environment_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 _wputenv,
                 const wchar_t *string)
  {
    int result = __ms__wputenv(string);
    if (result != 0)
      return result;

    // update UTF-8 environ
    const wchar_t *eq = wcschr(string, L'=');
    if (!eq) {
      _set_errno(EINVAL);
      return -1;
    }

    d::u_str key;
    if (!key.from_w(string, eq - string)) {
      _set_errno(ENOMEM);
      return -1;
    }

    d::u_str value;
    if (!value.from_w(eq + 1)) {
      _set_errno(ENOMEM);
      return -1;
    }

    internal::u8_environ_assign(key, value);
    return 0;
  }
} // namespace mingw_thunk
