#include "Process32NextW.h"

#include <thunk/_common.h>

#include <tlhelp32.h>
#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_Process32NextW(_In_ HANDLE hSnapshot,
                                        _Out_ LPPROCESSENTRY32W lppe);
  }

  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 Process32NextW,
                 _In_ HANDLE hSnapshot,
                 _Out_ LPPROCESSENTRY32W lppe)
  {
    __DISPATCH_THUNK_2(Process32NextW,
                       const auto pfn = try_get_Process32NextW(),
                       pfn,
                       &f::fallback_Process32NextW);

    return dllimport_Process32NextW(hSnapshot, lppe);
  }

  namespace f
  {
    BOOL WINAPI fallback_Process32NextW(_In_ HANDLE hSnapshot,
                                        _Out_ LPPROCESSENTRY32W lppe)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
