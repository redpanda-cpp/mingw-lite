#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, BOOL, WINAPI, IsDebuggerPresent)
  {
    if (const auto pfn = try_get_IsDebuggerPresent())
      return pfn();

    return FALSE;
  }
} // namespace mingw_thunk
