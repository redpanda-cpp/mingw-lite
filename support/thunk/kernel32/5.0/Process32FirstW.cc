#include "Process32FirstW.h"

#include <thunk/_common.h>

#include <tlhelp32.h>
#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_Process32FirstW(_In_ HANDLE hSnapshot,
                                         _Inout_ LPPROCESSENTRY32W lppe);
  }

  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 Process32FirstW,
                 _In_ HANDLE hSnapshot,
                 _Inout_ LPPROCESSENTRY32W lppe)
  {
    __DISPATCH_THUNK_2(Process32FirstW,
                       const auto pfn = try_get_Process32FirstW(),
                       pfn,
                       &f::fallback_Process32FirstW);

    return dllimport_Process32FirstW(hSnapshot, lppe);
  }

  namespace f
  {
    BOOL WINAPI fallback_Process32FirstW(_In_ HANDLE hSnapshot,
                                         _Inout_ LPPROCESSENTRY32W lppe)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
