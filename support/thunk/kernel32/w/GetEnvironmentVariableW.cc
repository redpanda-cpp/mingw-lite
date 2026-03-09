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
    __DISPATCH_THUNK_2(GetEnvironmentVariableW,
                       i::is_nt(),
                       &__ms_GetEnvironmentVariableW,
                       &f::win9x_GetEnvironmentVariableW);

    return dllimport_GetEnvironmentVariableW(lpName, lpBuffer, nSize);
  }

  namespace f
  {
    DWORD __stdcall win9x_GetEnvironmentVariableW(_In_opt_ LPCWSTR lpName,
                                                  _Out_opt_ LPWSTR lpBuffer,
                                                  _In_ DWORD nSize)
    {
      d::a_str a_name;
      if (lpName && !a_name.from_w(lpName)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }

      d::a_str a_buffer{d::max_path_tag{}};
      DWORD ret = __ms_GetEnvironmentVariableA(
          lpName ? a_name.c_str() : nullptr, a_buffer.data(), MAX_PATH);
      if (ret == 0) {
        return 0;
      }

      while (ret > a_buffer.size()) {
        if (!a_buffer.resize(ret)) {
          SetLastError(ERROR_OUTOFMEMORY);
          return 0;
        }
        ret = __ms_GetEnvironmentVariableA(
            lpName ? a_name.c_str() : nullptr, a_buffer.data(), ret);
        if (ret == 0)
          return 0;
      }

      d::w_str w_buffer;
      if (!w_buffer.from_a(a_buffer.c_str(), ret)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      size_t w_size = w_buffer.size();
      if (w_size + 1 > static_cast<size_t>(nSize)) {
        return w_size + 1;
      }

      c::wmemcpy(lpBuffer, w_buffer.c_str(), w_size);
      lpBuffer[w_size] = 0;
      return w_size;
    }
  } // namespace f
} // namespace mingw_thunk
