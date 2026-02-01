#include "GetEnvironmentVariableW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <processenv.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 12,
                 DWORD,
                 WINAPI,
                 GetEnvironmentVariableW,
                 _In_opt_ LPCWSTR lpName,
                 _Out_opt_ LPWSTR lpBuffer,
                 _In_ DWORD nSize)
  {
    if (internal::is_nt())
      return __ms_GetEnvironmentVariableW(lpName, lpBuffer, nSize);

    return impl::win9x_GetEnvironmentVariableW(lpName, lpBuffer, nSize);
  }

  namespace impl
  {
    DWORD
    win9x_GetEnvironmentVariableW(_In_opt_ LPCWSTR lpName,
                                  _Out_opt_ LPWSTR lpBuffer,
                                  _In_ DWORD nSize)
    {
      stl::string a_name;
      if (lpName)
        a_name = internal::w2a(lpName);

      constexpr size_t max_env_size = 32767;
      char a_buffer[max_env_size + 1] = {0};
      DWORD ret = __ms_GetEnvironmentVariableA(
          lpName ? a_name.c_str() : nullptr, a_buffer, max_env_size + 1);
      if (ret == 0) {
        return 0;
      }
      if (ret > max_env_size) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
      }

      stl::wstring w_buffer = internal::a2w(a_buffer, ret);
      if (w_buffer.size() + 1 > nSize) {
        return w_buffer.size() + 1;
      }

      libc::wmemcpy(lpBuffer, w_buffer.c_str(), w_buffer.size());
      lpBuffer[w_buffer.size()] = L'\0';
      return w_buffer.size();
    }
  } // namespace impl
} // namespace mingw_thunk
