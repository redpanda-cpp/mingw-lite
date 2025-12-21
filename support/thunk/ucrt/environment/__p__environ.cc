#include "__p__environ.h"

#include <thunk/_common.h>
#include <thunk/string.h>

#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      api_ms_win_crt_environment_l1_1_0, 0, char ***, __cdecl, __p__environ)
  {
    while (!__atomic_test_and_set(&internal::u8_environ_lock, __ATOMIC_ACQUIRE))
      Sleep(0);

    if (internal::u8_environ == nullptr) {
      internal::u8_environ = (char **)malloc(sizeof(char *));
      internal::u8_environ[0] = nullptr;
    };

    __atomic_clear(&internal::u8_environ_lock, __ATOMIC_RELEASE);
    return &internal::u8_environ;
  }

  namespace internal
  {
    char **u8_dup_envp(char **envp, size_t size)
    {
      char **new_environ = (char **)malloc(sizeof(char *) * (size + 1));
      for (size_t i = 0; i < size; ++i)
        new_environ[i] = _strdup(envp[i]);
      new_environ[size] = nullptr;
      return new_environ;
    }

    char **u8_envp = nullptr;

    char **u8_environ = nullptr;
    size_t u8_environ_size = 0;
    bool u8_environ_lock = false;
  } // namespace internal
} // namespace mingw_thunk
