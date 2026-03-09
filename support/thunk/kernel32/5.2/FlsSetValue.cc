#include "FlsSetValue.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 FlsSetValue,
                 _In_ DWORD dwFlsIndex,
                 _In_opt_ PVOID lpFlsData)
  {
    __DISPATCH_THUNK_2(FlsSetValue,
                       const auto pfn = try_get_FlsSetValue(),
                       pfn,
                       &f::fallback_FlsSetValue);

    return dllimport_FlsSetValue(dwFlsIndex, lpFlsData);
  }

  namespace f
  {
    BOOL __stdcall fallback_FlsSetValue(_In_ DWORD dwFlsIndex,
                                        _In_opt_ PVOID lpFlsData)
    {
      return TlsSetValue(dwFlsIndex, lpFlsData);
    }
  } // namespace f
} // namespace mingw_thunk
