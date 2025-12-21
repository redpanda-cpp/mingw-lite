#include <thunk/_common.h>
#include <thunk/_no_thunk.h>

#define PSAPI_VERSION 2

#include <psapi.h>
#include <windows.h>

extern "C" decltype(::K32GetModuleFileNameExW) __ms_GetModuleFileNameExW;

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 16,
                 DWORD,
                 WINAPI,
                 K32GetModuleFileNameExW,
                 _In_ HANDLE hProcess,
                 _In_opt_ HMODULE hModule,
                 _Out_ LPWSTR lpFilename,
                 _In_ DWORD nSize)
  {
    if (const auto pfn = try_get_K32GetModuleFileNameExW())
      return pfn(hProcess, hModule, lpFilename, nSize);

#if THUNK_LEVEL >= NTDDI_WINXP

    return __ms_GetModuleFileNameExW(hProcess, hModule, lpFilename, nSize);

#else

    if (const auto pfn = internal::module_psapi()
                             .get_function<decltype(::K32GetModuleFileNameExW)>(
                                 "GetModuleFileNameExW"))
      return pfn(hProcess, hModule, lpFilename, nSize);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;

#endif
  }
} // namespace mingw_thunk
