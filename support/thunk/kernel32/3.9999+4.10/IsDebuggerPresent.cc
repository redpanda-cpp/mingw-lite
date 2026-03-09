#include "IsDebuggerPresent.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, BOOL, WINAPI, IsDebuggerPresent)
  {
    __DISPATCH_THUNK_2(IsDebuggerPresent,
                       const auto pfn = try_get_IsDebuggerPresent(),
                       pfn,
                       &f::fallback_IsDebuggerPresent);

    return dllimport_IsDebuggerPresent();
  }

  namespace f
  {
    BOOL __stdcall fallback_IsDebuggerPresent(void)
    {
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
