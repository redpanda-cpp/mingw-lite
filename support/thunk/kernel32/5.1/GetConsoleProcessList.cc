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
    if (const auto pfn = try_get_GetConsoleProcessList())
      return pfn(lpdwProcessList, dwProcessCount);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
  }
} // namespace mingw_thunk
