#include "SetProcessAffinityMask.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 SetProcessAffinityMask,
                 _In_ HANDLE hProcess,
                 _In_ DWORD_PTR dwProcessAffinityMask)
  {
    __DISPATCH_THUNK_2(SetProcessAffinityMask,
                       const auto pfn = try_get_SetProcessAffinityMask(),
                       pfn,
                       &f::fallback_SetProcessAffinityMask);

    return dllimport_SetProcessAffinityMask(hProcess, dwProcessAffinityMask);
  }

  namespace f
  {
    BOOL __stdcall
    fallback_SetProcessAffinityMask(_In_ HANDLE hProcess,
                                    _In_ DWORD_PTR dwProcessAffinityMask)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
