#include "CreateToolhelp32Snapshot.h"

#include <thunk/_common.h>

#include <tlhelp32.h>
#include <windows.h>
#include <winerror.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 HANDLE,
                 WINAPI,
                 CreateToolhelp32Snapshot,
                 _In_ DWORD dwFlags,
                 _In_ DWORD th32ProcessID)
  {
    __DISPATCH_THUNK_2(CreateToolhelp32Snapshot,
                       const auto pfn = try_get_CreateToolhelp32Snapshot(),
                       pfn,
                       &f::fallback_CreateToolhelp32Snapshot);

    return dllimport_CreateToolhelp32Snapshot(dwFlags, th32ProcessID);
  }

  namespace f
  {
    HANDLE __stdcall fallback_CreateToolhelp32Snapshot(_In_ DWORD dwFlags,
                                                       _In_ DWORD th32ProcessID)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return INVALID_HANDLE_VALUE;
    }
  } // namespace f
} // namespace mingw_thunk
