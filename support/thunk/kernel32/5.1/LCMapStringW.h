#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    int winnt_LCMapStringW(_In_ LCID Locale,
                           _In_ DWORD dwMapFlags,
                           _In_ LPCWSTR lpSrcStr,
                           _In_ int cchSrc,
                           _Out_opt_ LPWSTR lpDestStr,
                           _In_ int cchDest);

    int win9x_LCMapStringW(_In_ LCID Locale,
                           _In_ DWORD dwMapFlags,
                           _In_ LPCWSTR lpSrcStr,
                           _In_ int cchSrc,
                           _Out_opt_ LPWSTR lpDestStr,
                           _In_ int cchDest);
  } // namespace impl
} // namespace mingw_thunk
