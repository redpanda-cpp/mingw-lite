#include "SetEnvironmentVariableW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
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
    __DISPATCH_THUNK_2(SetEnvironmentVariableW,
                       i::is_nt(),
                       &__ms_SetEnvironmentVariableW,
                       &f::win9x_SetEnvironmentVariableW);

    return dllimport_SetEnvironmentVariableW(lpName, lpValue);
  }

  namespace f
  {
    BOOL __stdcall win9x_SetEnvironmentVariableW(_In_ LPCWSTR lpName,
                                                 _In_opt_ LPCWSTR lpValue)
    {
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
  } // namespace f
} // namespace mingw_thunk
