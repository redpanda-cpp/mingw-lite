#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <processenv.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 SetEnvironmentVariableW,
                 _In_ LPCWSTR lpName,
                 _In_opt_ LPCWSTR lpValue)
  {
    if (internal::is_nt())
      return get_SetEnvironmentVariableW()(lpName, lpValue);

    stl::string a_name = internal::narrow(lpName);
    stl::string a_value;
    if (lpValue)
      a_value = internal::narrow(lpValue);

    return SetEnvironmentVariableA(a_name.c_str(),
                                   lpValue ? a_value.c_str() : nullptr);
  }
} // namespace mingw_thunk
