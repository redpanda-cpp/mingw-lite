#include <thunk/_common.h>

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
    if (const auto pfn = try_get_SetProcessAffinityMask())
      return pfn(hProcess, dwProcessAffinityMask);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
} // namespace mingw_thunk
