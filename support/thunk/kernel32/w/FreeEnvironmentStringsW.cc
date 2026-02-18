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
    if (internal::is_nt())
      return __ms_FreeEnvironmentStringsW(penv);

    if (penv)
      HeapFree(GetProcessHeap(), 0, penv);

    return TRUE;
  }
} // namespace mingw_thunk
