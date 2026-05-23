#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    LPVOID __stdcall
    fallback_MapViewOfFileExNuma(_In_ HANDLE hFileMappingObject,
                                 _In_ DWORD dwDesiredAccess,
                                 _In_ DWORD dwFileOffsetHigh,
                                 _In_ DWORD dwFileOffsetLow,
                                 _In_ SIZE_T dwNumberOfBytesToMap,
                                 _In_opt_ LPVOID lpBaseAddress,
                                 _In_ DWORD nndPreferred);
  }
} // namespace mingw_thunk
