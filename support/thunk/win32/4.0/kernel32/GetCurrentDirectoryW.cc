#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

#include <nocrt/__wchar/wmemcpy.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 DWORD,
                 WINAPI,
                 GetCurrentDirectoryW,
                 _In_ DWORD nBufferLength,
                 _Out_ LPWSTR lpBuffer)
  {
    if (internal::is_nt())
      return get_GetCurrentDirectoryW()(nBufferLength, lpBuffer);

    char a_filename[MAX_PATH];
    DWORD a_size = GetCurrentDirectoryA(MAX_PATH, a_filename);
    if (a_size == 0)
      return 0;
    stl::wstring w_filename = internal::widen(a_filename);
    if (nBufferLength >= w_filename.size() + 1) {
      wmemcpy(lpBuffer, w_filename.c_str(), w_filename.size());
      lpBuffer[w_filename.size()] = L'\0';
      return w_filename.size();
    } else {
      return w_filename.size() + 1;
    }
  }
} // namespace mingw_thunk
