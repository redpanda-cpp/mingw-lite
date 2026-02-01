#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 16,
                 HANDLE,
                 WINAPI,
                 CreateSemaphoreW,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
                 _In_ LONG lInitialCount,
                 _In_ LONG lMaximumCount,
                 _In_opt_ LPCWSTR lpName)
  {
    if (internal::is_nt())
      return __ms_CreateSemaphoreW(
          lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);

    stl::string a_name;
    if (lpName)
      a_name = internal::w2a(lpName);
    return __ms_CreateSemaphoreA(lpSemaphoreAttributes,
                                 lInitialCount,
                                 lMaximumCount,
                                 lpName ? a_name.c_str() : nullptr);
  }
} // namespace mingw_thunk
