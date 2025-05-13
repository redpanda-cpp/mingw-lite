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
                 RegDeleteKeyExA,
                 _In_ HKEY hKey,
                 _In_ LPCSTR lpSubKey,
                 _In_ REGSAM samDesired,
                 _Reserved_ DWORD Reserved)
  {
    if (auto const pRegDeleteKeyExA = try_get_RegDeleteKeyExA()) {
      return pRegDeleteKeyExA(hKey, lpSubKey, samDesired, Reserved);
    }

    return RegDeleteKeyA(hKey, lpSubKey);
  }
} // namespace mingw_thunk
