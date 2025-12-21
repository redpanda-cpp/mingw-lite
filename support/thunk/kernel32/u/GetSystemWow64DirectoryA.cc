#include <thunk/_common.h>

#include <windows.h>
#include <winnls.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 UINT,
                 WINAPI,
                 GetSystemWow64DirectoryA,
                 _Out_ LPSTR lpBuffer,
                 _In_ UINT uSize)
  {
    wchar_t w_buffer[MAX_PATH];
    DWORD w_size = GetSystemWow64DirectoryW(w_buffer, MAX_PATH);
    if (!w_size)
      return 0;
    if (w_size > MAX_PATH) {
      SetLastError(ERROR_FILENAME_EXCED_RANGE);
      return 0;
    }

    int len = WideCharToMultiByte(
        CP_UTF8, 0, w_buffer, w_size, nullptr, 0, nullptr, nullptr);
    if (len >= uSize) {
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      return len + 1;
    }

    WideCharToMultiByte(
        CP_UTF8, 0, w_buffer, w_size, lpBuffer, len, nullptr, nullptr);
    lpBuffer[len] = 0;
    return len;
  }
} // namespace mingw_thunk
