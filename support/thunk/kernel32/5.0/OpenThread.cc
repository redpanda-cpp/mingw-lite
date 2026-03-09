#include "OpenThread.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE WINAPI fallback_OpenThread(_In_ DWORD dwDesiredAccess,
                                      _In_ BOOL bInheritHandle,
                                      _In_ DWORD dwThreadId);
  }

  __DEFINE_THUNK(kernel32,
                 12,
                 HANDLE,
                 WINAPI,
                 OpenThread,
                 _In_ DWORD dwDesiredAccess,
                 _In_ BOOL bInheritHandle,
                 _In_ DWORD dwThreadId)
  {
    __DISPATCH_THUNK_2(OpenThread,
                       const auto pfn = try_get_OpenThread(),
                       pfn,
                       &f::fallback_OpenThread);

    return dllimport_OpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
  }

  namespace f
  {
    HANDLE WINAPI fallback_OpenThread(_In_ DWORD dwDesiredAccess,
                                      _In_ BOOL bInheritHandle,
                                      _In_ DWORD dwThreadId)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return NULL;
    }
  } // namespace f
} // namespace mingw_thunk
