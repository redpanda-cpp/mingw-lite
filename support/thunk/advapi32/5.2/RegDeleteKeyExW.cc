#include "RegDeleteKeyExW.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Windows XP Professional x64 Edition, Windows Server 2003 with SP1
  __DEFINE_THUNK(advapi32,
                 16,
                 LSTATUS,
                 APIENTRY,
                 RegDeleteKeyExW,
                 _In_ HKEY hKey,
                 _In_ LPCWSTR lpSubKey,
                 _In_ REGSAM samDesired,
                 _Reserved_ DWORD Reserved)
  {
    __DISPATCH_THUNK_2(RegDeleteKeyExW,
                       const auto pfn = try_get_RegDeleteKeyExW(),
                       pfn,
                       &f::fallback_RegDeleteKeyExW);

    return dllimport_RegDeleteKeyExW(hKey, lpSubKey, samDesired, Reserved);
  }

  namespace f
  {
    LSTATUS __stdcall fallback_RegDeleteKeyExW(_In_ HKEY hKey,
                                               _In_ LPCWSTR lpSubKey,
                                               _In_ REGSAM samDesired,
                                               _Reserved_ DWORD Reserved)
    {
      return RegDeleteKeyW(hKey, lpSubKey);
    }
  } // namespace f
} // namespace mingw_thunk
