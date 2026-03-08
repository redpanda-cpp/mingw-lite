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
    // If the function fails, the return value is a nonzero error code defined
    // in Winerror.h.
    if (!lpSubKey)
      return ERROR_INVALID_PARAMETER;

    d::w_str w_sub_key;
    if (!w_sub_key.from_u(lpSubKey))
      return ERROR_OUTOFMEMORY;

    return RegDeleteKeyExW(hKey, w_sub_key.c_str(), samDesired, Reserved);
  }
} // namespace mingw_thunk
