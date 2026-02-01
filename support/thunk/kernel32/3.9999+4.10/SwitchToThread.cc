#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, BOOL, WINAPI, SwitchToThread)
  {
    if (const auto pfn = try_get_SwitchToThread())
      return pfn();

    Sleep(0);
    return TRUE;
  }
} // namespace mingw_thunk
