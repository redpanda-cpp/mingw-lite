#include "FlsAlloc.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 DWORD,
                 WINAPI,
                 FlsAlloc,
                 _In_opt_ PFLS_CALLBACK_FUNCTION lpCallback)
  {
    __DISPATCH_THUNK_2(FlsAlloc,
                       const auto pfn = try_get_FlsAlloc(),
                       pfn,
                       &f::fallback_FlsAlloc);

    return dllimport_FlsAlloc(lpCallback);
  }

  namespace f
  {
    DWORD __stdcall
    fallback_FlsAlloc(_In_opt_ PFLS_CALLBACK_FUNCTION lpCallback)
    {
      // UCRT ignores the callback function, so do we.
      return TlsAlloc();
    }
  } // namespace f
} // namespace mingw_thunk
