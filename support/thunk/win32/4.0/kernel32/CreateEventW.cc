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
                 CreateEventW,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
                 _In_ BOOL bManualReset,
                 _In_ BOOL bInitialState,
                 _In_opt_ LPCWSTR lpName)
  {
    if (internal::is_nt())
      return get_CreateEventW()(
          lpEventAttributes, bManualReset, bInitialState, lpName);

    stl::string a_name;
    if (lpName)
      a_name = internal::narrow(lpName);
    return CreateEventA(lpEventAttributes,
                        bManualReset,
                        bInitialState,
                        lpName ? a_name.c_str() : nullptr);
  }
} // namespace mingw_thunk
