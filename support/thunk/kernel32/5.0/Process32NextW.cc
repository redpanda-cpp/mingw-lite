#include <thunk/_common.h>

#include <tlhelp32.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 Process32NextW,
                 _In_ HANDLE hSnapshot,
                 _Out_ LPPROCESSENTRY32W lppe)
  {
    if (const auto pfn = try_get_Process32NextW())
      return pfn(hSnapshot, lppe);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
} // namespace mingw_thunk
