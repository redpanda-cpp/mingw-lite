#include "GetConsoleWindow.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HWND WINAPI fallback_GetConsoleWindow();
  }

  __DEFINE_THUNK(kernel32, 0, HWND, WINAPI, GetConsoleWindow)
  {
    __DISPATCH_THUNK_2(GetConsoleWindow,
                       const auto pfn = try_get_GetConsoleWindow(),
                       pfn,
                       &f::fallback_GetConsoleWindow);

    return dllimport_GetConsoleWindow();
  }

  namespace f
  {
    HWND WINAPI fallback_GetConsoleWindow()
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return nullptr;
    }
  } // namespace f
} // namespace mingw_thunk
