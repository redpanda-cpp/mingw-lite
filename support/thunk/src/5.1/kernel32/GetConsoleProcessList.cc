#include <thunk/_common.h>

#include <windows.h>
#include <winerror.h>

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
    if (const auto pfn = try_get_GetConsoleProcessList())
      return pfn(lpdwProcessList, dwProcessCount);

    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
  }
} // namespace mingw_thunk
