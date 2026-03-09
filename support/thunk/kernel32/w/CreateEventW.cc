#include "CreateEventW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 16,
                 HANDLE,
                 WINAPI,
                 CreateEventW,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
                 _In_ BOOL bManualReset,
                 _In_ BOOL bInitialState,
                 _In_opt_ LPCWSTR lpName)
  {
    __DISPATCH_THUNK_2(
        CreateEventW, i::is_nt(), &__ms_CreateEventW, &f::win9x_CreateEventW);

    return dllimport_CreateEventW(
        lpEventAttributes, bManualReset, bInitialState, lpName);
  }

  namespace f
  {
    HANDLE __stdcall
    win9x_CreateEventW(_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
                       _In_ BOOL bManualReset,
                       _In_ BOOL bInitialState,
                       _In_opt_ LPCWSTR lpName)
    {
      d::a_str a_name;
      if (lpName && !a_name.from_w(lpName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return nullptr;
      }

      return __ms_CreateEventA(lpEventAttributes,
                               bManualReset,
                               bInitialState,
                               lpName ? a_name.c_str() : nullptr);
    }
  } // namespace f
} // namespace mingw_thunk
