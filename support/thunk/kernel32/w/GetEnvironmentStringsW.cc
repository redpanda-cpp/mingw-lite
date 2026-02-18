#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <processenv.h>
#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32, 0, LPWCH, WINAPI, GetEnvironmentStringsW)
  {
    if (internal::is_nt())
      return __ms_GetEnvironmentStringsW();

    char *a_env = __ms_GetEnvironmentStrings();
    if (!a_env)
      return nullptr;

    size_t a_size = 0;
    while (a_env[a_size]) {
      size_t l = strlen(a_env + a_size);
      a_size += l + 1;
    }
    a_size += 1;

    int w_size = __ms_MultiByteToWideChar(CP_ACP, 0, a_env, a_size, nullptr, 0);
    wchar_t *w_env =
        (wchar_t *)HeapAlloc(GetProcessHeap(), 0, w_size * sizeof(wchar_t));
    if (!w_env) {
      __ms_FreeEnvironmentStringsA(a_env);
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return nullptr;
    }

    __ms_MultiByteToWideChar(CP_ACP, 0, a_env, a_size, w_env, w_size);
    __ms_FreeEnvironmentStringsA(a_env);
    return w_env;
  }
} // namespace mingw_thunk
