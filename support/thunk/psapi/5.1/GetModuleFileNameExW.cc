#define PSAPI_VERSION 1

#include "GetModuleFileNameExW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>

#include <psapi.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(psapi,
                 16,
                 DWORD,
                 WINAPI,
                 GetModuleFileNameExW,
                 _In_ HANDLE hProcess,
                 _In_opt_ HMODULE hModule,
                 _Out_ LPWSTR lpFilename,
                 _In_ DWORD nSize)
  {
    __DISPATCH_THUNK_2(GetModuleFileNameExW,
                       const auto pfn = try_get_GetModuleFileNameExW(),
                       pfn,
                       &f::fallback_GetModuleFileNameExW);

    return dllimport_GetModuleFileNameExW(hProcess, hModule, lpFilename, nSize);
  }

  namespace f
  {
    DWORD __stdcall fallback_GetModuleFileNameExW(_In_ HANDLE hProcess,
                                                  _In_opt_ HMODULE hModule,
                                                  _Out_ LPWSTR lpFilename,
                                                  _In_ DWORD nSize)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
