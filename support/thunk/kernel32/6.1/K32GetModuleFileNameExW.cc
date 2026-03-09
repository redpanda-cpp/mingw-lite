#include "K32GetModuleFileNameExW.h"

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
#if THUNK_LEVEL >= NTDDI_WINXP
    __DISPATCH_THUNK_2(K32GetModuleFileNameExW,
                       const auto pfn = try_get_K32GetModuleFileNameExW(),
                       pfn,
                       &__ms_GetModuleFileNameExW);
#else
    __DISPATCH_THUNK_3(
        K32GetModuleFileNameExW,
        const auto pfn = try_get_K32GetModuleFileNameExW(),
        pfn,
        const auto psapi_fn =
            internal::module_psapi()
                .get_function<decltype(::K32GetModuleFileNameExW)>(
                    "GetModuleFileNameExW"),
        psapi_fn,
        &f::prexp_K32GetModuleFileNameExW);
#endif

    return dllimport_K32GetModuleFileNameExW(
        hProcess, hModule, lpFilename, nSize);
  }

  namespace f
  {
    DWORD __stdcall prexp_K32GetModuleFileNameExW(_In_ HANDLE hProcess,
                                                  _In_opt_ HMODULE hModule,
                                                  _Out_ LPWSTR lpFilename,
                                                  _In_ DWORD nSize)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
