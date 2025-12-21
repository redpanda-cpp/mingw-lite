#include "__p__environ.h"

#include <thunk/_common.h>
#include <thunk/string.h>

#include <stdlib.h>
#include <string.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_environment_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 _putenv,
                 const char *string)
  {
    using internal::u8_environ;
    using internal::u8_environ_size;

    stl::wstring w_string = internal::u2w(string);
    int result = _wputenv(w_string.c_str());
    if (result != 0)
      return result;

    const char *eq = strchr(string, '=');
    if (!eq) [[unlikely]] {
      // should have beed checked by _wputenv
      _set_errno(EINVAL);
      return -1;
    }

    size_t name_len = eq - string;
    size_t value_len = strlen(eq + 1);

    while (!__atomic_test_and_set(&internal::u8_environ_lock, __ATOMIC_ACQUIRE))
      Sleep(0);

    if (u8_environ == internal::u8_envp)
      u8_environ = internal::u8_dup_envp(u8_environ, u8_environ_size);

    if (value_len == 0) {
      // remove
      for (int i = 0; i < u8_environ_size; i++) {
        if (_strnicmp(u8_environ[i], string, name_len) == 0 &&
            u8_environ[i][name_len] == '=') {
          free(u8_environ[i]);
          for (int j = i; j < u8_environ_size - 1; j++)
            u8_environ[j] = u8_environ[j + 1];
          u8_environ[u8_environ_size - 1] = nullptr;
          u8_environ_size--;
          break;
        }
      }
    } else {
      char *new_entry = (char *)malloc(name_len + 1 + value_len + 1);
      memcpy(new_entry, string, name_len + 1 + value_len);
      new_entry[name_len + 1 + value_len] = 0;

      // update?
      int i = 0;
      for (; i < u8_environ_size; i++) {
        if (_strnicmp(u8_environ[i], string, name_len) == 0 &&
            u8_environ[i][name_len] == '=') {
          free(u8_environ[i]);
          u8_environ[i] = new_entry;
          break;
        }
      }

      // add
      if (i == u8_environ_size) {
        char **new_envp = (char **)realloc(
            u8_environ, sizeof(char *) * (u8_environ_size + 2));
        new_envp[u8_environ_size] = new_entry;
        new_envp[u8_environ_size + 1] = nullptr;
        u8_environ = new_envp;
        u8_environ_size++;
      }
    }

    __atomic_clear(&internal::u8_environ_lock, __ATOMIC_RELEASE);
    return 0;
  }
} // namespace mingw_thunk
