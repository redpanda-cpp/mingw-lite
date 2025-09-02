#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errhandlingapi.h>
#include <synchapi.h>
#include <winerror.h>

// CreateWaitableTimerW is not declared in synchapi.h
#include <windows.h>

#include <nostl/string.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 HANDLE,
                 WINAPI,
                 CreateWaitableTimerW,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                 _In_ BOOL bManualReset,
                 _In_opt_ LPCWSTR lpTimerName)
  {
    if (internal::is_nt())
      return get_CreateWaitableTimerW()(
          lpTimerAttributes, bManualReset, lpTimerName);

    stl::string aname = internal::narrow(lpTimerName);
    return CreateWaitableTimerA(lpTimerAttributes, bManualReset, aname.c_str());
  }
} // namespace mingw_thunk
