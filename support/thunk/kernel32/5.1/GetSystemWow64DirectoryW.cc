#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 UINT,
                 WINAPI,
                 GetSystemWow64DirectoryW,
                 _Out_ LPWSTR lpBuffer,
                 _In_ UINT uSize)
  {
    if (const auto pfn = try_get_GetSystemWow64DirectoryW())
      return pfn(lpBuffer, uSize);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
  }
} // namespace mingw_thunk
