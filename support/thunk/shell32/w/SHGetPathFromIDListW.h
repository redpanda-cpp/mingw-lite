#pragma once

#include <shlobj.h>
#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_SHGetPathFromIDListW(_In_ PCIDLIST_ABSOLUTE pidl,
                                              _Out_ LPWSTR pszPath);
  }
} // namespace mingw_thunk
