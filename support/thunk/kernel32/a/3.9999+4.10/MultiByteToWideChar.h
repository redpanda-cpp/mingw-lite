#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    int win95_MultiByteToWideChar(_In_ UINT CodePage,
                                  _In_ DWORD dwFlags,
                                  _In_ LPCSTR lpMultiByteStr,
                                  _In_ int cbMultiByte,
                                  _Out_opt_ LPWSTR lpWideCharStr,
                                  _In_ int cchWideChar);
  }
} // namespace mingw_thunk
