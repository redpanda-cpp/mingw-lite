#include "FlsGetValue.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 4, PVOID, WINAPI, FlsGetValue, _In_ DWORD dwFlsIndex)
  {
    __DISPATCH_THUNK_2(FlsGetValue,
                       const auto pfn = try_get_FlsGetValue(),
                       pfn,
                       &f::fallback_FlsGetValue);

    return dllimport_FlsGetValue(dwFlsIndex);
  }

  namespace f
  {
    PVOID __stdcall fallback_FlsGetValue(_In_ DWORD dwFlsIndex)
    {
      return TlsGetValue(dwFlsIndex);
    }
  } // namespace f
} // namespace mingw_thunk
