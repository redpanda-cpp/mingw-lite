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
    stl::wstring w_sub_key = internal::u2w(lpSubKey);
    return RegDeleteKeyExW(hKey, w_sub_key.c_str(), samDesired, Reserved);
  }
} // namespace mingw_thunk
