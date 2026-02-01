#include <thunk/_common.h>

#include <windows.h>

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
    if (const auto pfn = try_get_GetSystemWow64DirectoryA())
      return pfn(lpBuffer, uSize);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
  }
} // namespace mingw_thunk
