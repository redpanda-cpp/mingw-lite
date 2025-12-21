#include "__p__environ.h"

#include <thunk/_common.h>
#include <thunk/string.h>

#include <stdlib.h>
#include <string.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_environment_l1_1_0,
                 0,
                 char *,
                 __cdecl,
                 getenv,
                 const char *name)
  {
    using internal::u8_environ;
    using internal::u8_environ_size;

    size_t name_len = strlen(name);
    for (int i = 0; i < u8_environ_size; i++) {
      char *entry = u8_environ[i];
      if (_strnicmp(entry, name, name_len) == 0 && entry[name_len] == '=')
        return entry + name_len + 1;
    }
    return nullptr;
  }
} // namespace mingw_thunk
