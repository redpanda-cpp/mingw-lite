#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    int win95_WideCharToMultiByte(_In_ UINT CodePage,
                                  _In_ DWORD dwFlags,
                                  _In_ LPCWCH lpWideCharStr,
                                  _In_ int cchWideChar,
                                  _Out_opt_ LPSTR lpMultiByteStr,
                                  _In_ int cbMultiByte,
                                  _In_opt_ LPCCH lpDefaultChar,
                                  _In_opt_ LPBOOL lpUsedDefaultChar);
  }
} // namespace mingw_thunk
