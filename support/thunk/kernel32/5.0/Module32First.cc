#include "Module32First.h"

#include <thunk/_common.h>

#include <tlhelp32.h>
#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_Module32First(_In_ HANDLE hSnapshot,
                                       _Inout_ LPMODULEENTRY32 lpme);
  }

  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 Module32First,
                 _In_ HANDLE hSnapshot,
                 _Inout_ LPMODULEENTRY32 lpme)
  {
    __DISPATCH_THUNK_2(Module32First,
                       const auto pfn = try_get_Module32First(),
                       pfn,
                       &f::fallback_Module32First);

    return dllimport_Module32First(hSnapshot, lpme);
  }

  namespace f
  {
    BOOL WINAPI fallback_Module32First(_In_ HANDLE hSnapshot,
                                       _Inout_ LPMODULEENTRY32 lpme)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
