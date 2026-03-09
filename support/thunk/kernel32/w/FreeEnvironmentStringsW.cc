#include "FreeEnvironmentStringsW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <processenv.h>
#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32, 4, BOOL, WINAPI, FreeEnvironmentStringsW, LPWCH penv)
  {
    __DISPATCH_THUNK_2(FreeEnvironmentStringsW,
                       i::is_nt(),
                       &__ms_FreeEnvironmentStringsW,
                       &f::win9x_FreeEnvironmentStringsW);

    return dllimport_FreeEnvironmentStringsW(penv);
  }

  namespace f
  {
    BOOL __stdcall win9x_FreeEnvironmentStringsW(LPWCH penv)
    {
      if (penv)
        HeapFree(GetProcessHeap(), 0, penv);

      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
