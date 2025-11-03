#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
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
      return get_CreateSemaphoreW()(
          lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);

    stl::string a_name;
    if (lpName)
      a_name = internal::narrow(lpName);
    return CreateSemaphoreA(lpSemaphoreAttributes,
                            lInitialCount,
                            lMaximumCount,
                            lpName ? a_name.c_str() : nullptr);
  }
} // namespace mingw_thunk
