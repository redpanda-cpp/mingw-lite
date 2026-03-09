#include "SwitchToThread.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, BOOL, WINAPI, SwitchToThread)
  {
    __DISPATCH_THUNK_2(SwitchToThread,
                       const auto pfn = try_get_SwitchToThread(),
                       pfn,
                       &f::fallback_SwitchToThread);

    return dllimport_SwitchToThread();
  }

  namespace f
  {
    BOOL __stdcall fallback_SwitchToThread(void)
    {
      Sleep(0);
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
