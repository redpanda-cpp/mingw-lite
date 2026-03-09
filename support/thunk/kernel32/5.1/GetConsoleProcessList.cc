#include "GetConsoleProcessList.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 DWORD,
                 WINAPI,
                 GetConsoleProcessList,
                 _Out_ LPDWORD lpdwProcessList,
                 _In_ DWORD dwProcessCount)
  {
    __DISPATCH_THUNK_2(GetConsoleProcessList,
                       const auto pfn = try_get_GetConsoleProcessList(),
                       pfn,
                       &f::fallback_GetConsoleProcessList);

    return dllimport_GetConsoleProcessList(lpdwProcessList, dwProcessCount);
  }

  namespace f
  {
    DWORD __stdcall
    fallback_GetConsoleProcessList(_Out_ LPDWORD lpdwProcessList,
                                   _In_ DWORD dwProcessCount)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
