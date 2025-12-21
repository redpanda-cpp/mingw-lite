#include <thunk/_common.h>

#define PSAPI_VERSION 1

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
    if (const auto pfn = try_get_GetModuleFileNameExW())
      return pfn(hProcess, hModule, lpFilename, nSize);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
  }
} // namespace mingw_thunk
