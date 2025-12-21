#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOLEAN,
                 APIENTRY,
                 CreateSymbolicLinkA,
                 _In_ LPCSTR lpSymlinkFileName,
                 _In_ LPCSTR lpTargetFileName,
                 _In_ DWORD dwFlags)
  {
    stl::wstring w_symlink = internal::u2w(lpSymlinkFileName);
    stl::wstring w_target = internal::u2w(lpTargetFileName);
    return CreateSymbolicLinkW(w_symlink.c_str(), w_target.c_str(), dwFlags);
  }
} // namespace mingw_thunk
