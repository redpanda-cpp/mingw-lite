#include "Module32Next.h"

#include <thunk/_common.h>

#include <tlhelp32.h>
#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_Module32Next(_In_ HANDLE hSnapshot,
                                      _Out_ LPMODULEENTRY32 lpme);
  }

  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 Module32Next,
                 _In_ HANDLE hSnapshot,
                 _Out_ LPMODULEENTRY32 lpme)
  {
    __DISPATCH_THUNK_2(Module32Next,
                       const auto pfn = try_get_Module32Next(),
                       pfn,
                       &f::fallback_Module32Next);

    return dllimport_Module32Next(hSnapshot, lpme);
  }

  namespace f
  {
    BOOL WINAPI fallback_Module32Next(_In_ HANDLE hSnapshot,
                                      _Out_ LPMODULEENTRY32 lpme)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
