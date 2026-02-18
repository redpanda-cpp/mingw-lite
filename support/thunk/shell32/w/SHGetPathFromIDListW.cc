#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <shlobj.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(shell32,
                 8,
                 BOOL,
                 WINAPI,
                 SHGetPathFromIDListW,
                 _In_ PCIDLIST_ABSOLUTE pidl,
                 _Out_ LPWSTR pszPath)
  {
    if (internal::is_nt())
      return __ms_SHGetPathFromIDListW(pidl, pszPath);

    char a_path[MAX_PATH];
    if (!__ms_SHGetPathFromIDListA(pidl, a_path))
      return FALSE;

    auto w_res = internal::a2w(a_path);
    if (w_res.size() >= MAX_PATH) {
      SetLastError(ERROR_FILENAME_EXCED_RANGE);
      return FALSE;
    }

    libc::wmemcpy(pszPath, w_res.data(), w_res.size());
    pszPath[w_res.size()] = 0;
    return TRUE;
  }
} // namespace mingw_thunk
