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
      return __ms_SetEnvironmentVariableW(lpName, lpValue);

    if (!lpName) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    d::a_str a_name;
    if (!a_name.from_w(lpName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    d::a_str a_value;
    if (lpValue && !a_value.from_w(lpValue)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    return __ms_SetEnvironmentVariableA(a_name.c_str(),
                                        lpValue ? a_value.c_str() : nullptr);
  }
} // namespace mingw_thunk
