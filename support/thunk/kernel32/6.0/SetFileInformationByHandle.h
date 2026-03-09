#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_SetFileInformationByHandle(
        _In_ HANDLE hFile,
        _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
        _In_reads_bytes_(dwBufferSize) LPVOID lpFileInformation,
        _In_ DWORD dwBufferSize);
  }
} // namespace mingw_thunk
