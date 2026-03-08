#include "GetTempPathW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 8,
                 DWORD,
                 WINAPI,
                 GetTempPathW,
                 _In_ DWORD nBufferLength,
                 _Out_ LPWSTR lpBuffer)
  {
    if (internal::is_nt())
      return __ms_GetTempPathW(nBufferLength, lpBuffer);

    return impl::win9x_GetTempPathW(nBufferLength, lpBuffer);
  }

  namespace impl
  {
    DWORD
    win9x_GetTempPathW(_In_ DWORD nBufferLength, _Out_ LPWSTR lpBuffer)
    {
      // dry run for buffer size
      if (nBufferLength && !lpBuffer) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }

      d::a_str a_buffer{d::max_path_tag{}};
      DWORD ret = __ms_GetTempPathA(MAX_PATH, a_buffer.data());
      if (ret == 0)
        return 0;

      while (ret > a_buffer.size()) {
        if (!a_buffer.resize(ret)) {
          SetLastError(ERROR_OUTOFMEMORY);
          return 0;
        }
        ret = __ms_GetTempPathA(ret, a_buffer.data());
        if (ret == 0)
          return 0;
      }

      d::w_str w_buffer;
      if (!w_buffer.from_a(a_buffer.data(), ret)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      size_t w_size = w_buffer.size();
      if (w_size + 1 > nBufferLength) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return w_size + 1;
      }

      c::wmemcpy(lpBuffer, w_buffer.c_str(), w_size);
      lpBuffer[w_size] = 0;
      return w_size;
    }
  } // namespace impl
} // namespace mingw_thunk
