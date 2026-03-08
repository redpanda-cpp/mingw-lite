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

    if (!pszPath) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    char a_path[MAX_PATH];
    if (!__ms_SHGetPathFromIDListA(pidl, a_path))
      return FALSE;

    d::w_str w_res;
    if (!w_res.from_a(a_path)){
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    if (w_res.size() >= MAX_PATH) {
      SetLastError(ERROR_FILENAME_EXCED_RANGE);
      return FALSE;
    }

    c::wmemcpy(pszPath, w_res.c_str(), w_res.size());
    pszPath[w_res.size()] = 0;
    return TRUE;
  }
} // namespace mingw_thunk
