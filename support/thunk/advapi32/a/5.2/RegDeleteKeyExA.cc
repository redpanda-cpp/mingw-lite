#include "RegDeleteKeyExA.h"

#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(advapi32,
                 16,
                 LSTATUS,
                 APIENTRY,
                 RegDeleteKeyExA,
                 _In_ HKEY hKey,
                 _In_ LPCSTR lpSubKey,
                 _In_ REGSAM samDesired,
                 _Reserved_ DWORD Reserved)
  {
    __DISPATCH_THUNK_2(RegDeleteKeyExA,
                       const auto pfn = try_get_RegDeleteKeyExA(),
                       pfn,
                       &f::fallback_RegDeleteKeyExA);

    return dllimport_RegDeleteKeyExA(hKey, lpSubKey, samDesired, Reserved);
  }

  namespace f
  {
    LSTATUS __stdcall fallback_RegDeleteKeyExA(_In_ HKEY hKey,
                                               _In_ LPCSTR lpSubKey,
                                               _In_ REGSAM samDesired,
                                               _Reserved_ DWORD Reserved)
    {
      // If the function fails, the return value is a nonzero error code defined
      // in Winerror.h.
      if (!lpSubKey)
        return ERROR_INVALID_PARAMETER;

      d::w_str w_sub_key;
      if (!w_sub_key.from_a(lpSubKey))
        return ERROR_OUTOFMEMORY;

      return RegDeleteKeyExW(hKey, w_sub_key.c_str(), samDesired, Reserved);
    }
  } // namespace f
} // namespace mingw_thunk
