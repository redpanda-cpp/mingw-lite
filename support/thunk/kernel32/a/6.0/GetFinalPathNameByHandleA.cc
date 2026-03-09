#include "GetFinalPathNameByHandleA.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 16,
                 DWORD,
                 WINAPI,
                 GetFinalPathNameByHandleA,
                 _In_ HANDLE hFile,
                 _Out_ LPSTR lpszFilePath,
                 _In_ DWORD cchFilePath,
                 _In_ DWORD dwFlags)
  {
    __DISPATCH_THUNK_2(GetFinalPathNameByHandleA,
                       const auto pfn = try_get_GetFinalPathNameByHandleA(),
                       pfn,
                       &f::fallback_GetFinalPathNameByHandleA);

    return dllimport_GetFinalPathNameByHandleA(
        hFile, lpszFilePath, cchFilePath, dwFlags);
  }

  namespace f
  {
    DWORD __stdcall fallback_GetFinalPathNameByHandleA(_In_ HANDLE hFile,
                                                       _Out_ LPSTR lpszFilePath,
                                                       _In_ DWORD cchFilePath,
                                                       _In_ DWORD dwFlags)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
