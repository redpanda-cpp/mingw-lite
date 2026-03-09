#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    int __stdcall winnt_LCMapStringW(_In_ LCID Locale,
                                     _In_ DWORD dwMapFlags,
                                     _In_ LPCWSTR lpSrcStr,
                                     _In_ int cchSrc,
                                     _Out_opt_ LPWSTR lpDestStr,
                                     _In_ int cchDest);

    int __stdcall win9x_LCMapStringW(_In_ LCID Locale,
                                     _In_ DWORD dwMapFlags,
                                     _In_ LPCWSTR lpSrcStr,
                                     _In_ int cchSrc,
                                     _Out_opt_ LPWSTR lpDestStr,
                                     _In_ int cchDest);
  } // namespace f
} // namespace mingw_thunk
