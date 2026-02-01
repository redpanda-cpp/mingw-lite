#include <thunk/_common.h>

#include <tlhelp32.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 Module32First,
                 _In_ HANDLE hSnapshot,
                 _Inout_ LPMODULEENTRY32 lpme)
  {
    if (const auto pfn = try_get_Module32First())
      return pfn(hSnapshot, lpme);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
} // namespace mingw_thunk
