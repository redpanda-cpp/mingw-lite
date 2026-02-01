#include <thunk/_common.h>

#include <windows.h>

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

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return NULL;
  }
} // namespace mingw_thunk
