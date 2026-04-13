#include "GetModuleHandleExA.h"

#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 GetModuleHandleExA,
                 _In_ DWORD dwFlags,
                 _In_opt_ LPCSTR lpModuleName,
                 _Out_ HMODULE *phModule)
  {
    __DISPATCH_THUNK_2(GetModuleHandleExA,
                       const auto pfn = try_get_GetModuleHandleExA(),
                       pfn,
                       &f::fallback_GetModuleHandleExA);

    return dllimport_GetModuleHandleExA(dwFlags, lpModuleName, phModule);
  }

  namespace f
  {
    BOOL __stdcall fallback_GetModuleHandleExA(_In_ DWORD dwFlags,
                                               _In_opt_ LPCSTR lpModuleName,
                                               _Out_ HMODULE *phModule)
    {
      if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
        return GetModuleHandleExW(dwFlags, (LPCWSTR)lpModuleName, phModule);

      if (!lpModuleName)
        return GetModuleHandleExW(dwFlags, nullptr, phModule);

      d::w_str w_name;
      if (!w_name.from_a(lpModuleName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      return GetModuleHandleExW(dwFlags, w_name.c_str(), phModule);
    }
  } // namespace f
} // namespace mingw_thunk
