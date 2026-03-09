#include "FlsFree.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 4, BOOL, WINAPI, FlsFree, _In_ DWORD dwFlsIndex)
  {
    __DISPATCH_THUNK_2(
        FlsFree, const auto pfn = try_get_FlsFree(), pfn, &f::fallback_FlsFree);

    return dllimport_FlsFree(dwFlsIndex);
  }

  namespace f
  {
    BOOL __stdcall fallback_FlsFree(_In_ DWORD dwFlsIndex)
    {
      return TlsFree(dwFlsIndex);
    }
  } // namespace f
} // namespace mingw_thunk
