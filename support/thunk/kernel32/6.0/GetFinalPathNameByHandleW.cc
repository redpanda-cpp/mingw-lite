#include "GetFinalPathNameByHandleW.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 16,
                 DWORD,
                 WINAPI,
                 GetFinalPathNameByHandleW,
                 _In_ HANDLE hFile,
                 _Out_ LPWSTR lpszFilePath,
                 _In_ DWORD cchFilePath,
                 _In_ DWORD dwFlags)
  {
    __DISPATCH_THUNK_2(GetFinalPathNameByHandleW,
                       const auto pfn = try_get_GetFinalPathNameByHandleW(),
                       pfn,
                       &f::fallback_GetFinalPathNameByHandleW);

    return dllimport_GetFinalPathNameByHandleW(
        hFile, lpszFilePath, cchFilePath, dwFlags);
  }

  namespace f
  {
    DWORD __stdcall
    fallback_GetFinalPathNameByHandleW(_In_ HANDLE hFile,
                                       _Out_ LPWSTR lpszFilePath,
                                       _In_ DWORD cchFilePath,
                                       _In_ DWORD dwFlags)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
