#include <thunk/_common.h>

#include <tlhelp32.h>
#include <windows.h>
#include <winerror.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 HANDLE,
                 WINAPI,
                 OpenThread,
                 _In_ DWORD dwDesiredAccess,
                 _In_ BOOL bInheritHandle,
                 _In_ DWORD dwThreadId)
  {
    if (const auto pfn = try_get_OpenThread())
      return pfn(dwDesiredAccess, bInheritHandle, dwThreadId);

    SetLastError(ERROR_NOT_SUPPORTED);
    return NULL;
  }
} // namespace mingw_thunk
