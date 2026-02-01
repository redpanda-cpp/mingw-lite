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
    if (const auto pfn = try_get_CreateToolhelp32Snapshot())
      return pfn(dwFlags, th32ProcessID);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return INVALID_HANDLE_VALUE;
  }
} // namespace mingw_thunk
