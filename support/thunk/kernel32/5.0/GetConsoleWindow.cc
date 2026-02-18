#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, HWND, WINAPI, GetConsoleWindow)
  {
    if (const auto pfn = try_get_GetConsoleWindow())
      return pfn();

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return nullptr;
  }
} // namespace mingw_thunk
