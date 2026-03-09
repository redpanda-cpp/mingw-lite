#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    int __stdcall
    fallback_LCMapStringEx(_In_opt_ LPCWSTR lpLocaleName,
                           _In_ DWORD dwMapFlags,
                           _In_reads_(cchSrc) LPCWSTR lpSrcStr,
                           _In_ int cchSrc,
                           _Out_writes_opt_(cchDest) LPWSTR lpDestStr,
                           _In_ int cchDest,
                           _In_opt_ LPNLSVERSIONINFO lpVersionInformation,
                           _In_opt_ LPVOID lpReserved,
                           _In_opt_ LPARAM sortHandle);
  }
} // namespace mingw_thunk
