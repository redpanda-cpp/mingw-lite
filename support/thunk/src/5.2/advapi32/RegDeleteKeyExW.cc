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
    if (auto const pRegDeleteKeyExW = try_get_RegDeleteKeyExW()) {
      return pRegDeleteKeyExW(hKey, lpSubKey, samDesired, Reserved);
    }

    return RegDeleteKeyW(hKey, lpSubKey);
  }
} // namespace mingw_thunk
