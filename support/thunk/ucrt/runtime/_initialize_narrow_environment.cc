#include <thunk/_common.h>

#include <corecrt_startup.h>
#include <stdlib.h>

#include <windows.h>

#include "../environment/__p__environ.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_runtime_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 _initialize_narrow_environment)
  {
    // BEWARE: runtime is not fully initialized yet!
    _initialize_wide_environment();

    wchar_t **wenvp = *__p__wenviron();

    int envc = 0;
    while (wenvp[envc])
      envc++;

    char **envp = (char **)malloc(sizeof(char *) * (envc + 1));
    for (int i = 0; i < envc; i++) {
      int len = WideCharToMultiByte(
          CP_UTF8, 0, wenvp[i], -1, nullptr, 0, nullptr, nullptr);
      envp[i] = (char *)malloc(len);
      WideCharToMultiByte(
          CP_UTF8, 0, wenvp[i], -1, envp[i], len, nullptr, nullptr);
    }
    envp[envc] = nullptr;

    internal::u8_envp = envp;

    __atomic_test_and_set(&internal::u8_environ_lock, __ATOMIC_ACQUIRE);
    internal::u8_environ = envp;
    internal::u8_environ_size = envc;
    __atomic_clear(&internal::u8_environ_lock, __ATOMIC_RELEASE);

    return 0;
  }
} // namespace mingw_thunk
