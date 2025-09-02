#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 DWORD,
                 WINAPI,
                 GetTempPathW,
                 _In_ DWORD nBufferLength,
                 _Out_ LPWSTR lpBuffer)
  {
    if (internal::is_nt())
      return get_GetTempPathW()(nBufferLength, lpBuffer);

    // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettemppathw
    // The maximum possible return value is MAX_PATH+1 (261).
    char a_buffer[MAX_PATH + 1];
    DWORD size = GetTempPathA(MAX_PATH + 1, a_buffer);
    if (size == 0)
      return 0;

    stl::wstring w_str = internal::widen(a_buffer, size);
    if (nBufferLength >= w_str.size() + 1) {
      libc::wmemcpy(lpBuffer, w_str.c_str(), w_str.size());
      lpBuffer[w_str.size()] = L'\0';
      return w_str.size();
    } else {
      return w_str.size() + 1;
    }
  }
} // namespace mingw_thunk
