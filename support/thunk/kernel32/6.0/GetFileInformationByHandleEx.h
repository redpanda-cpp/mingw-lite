#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall winnt_GetFileInformationByHandleEx(
        _In_ HANDLE hFile,
        _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
        _Out_writes_bytes_(dwBufferSize) LPVOID lpFileInformation,
        _In_ DWORD dwBufferSize);

    BOOL __stdcall win9x_GetFileInformationByHandleEx(
        _In_ HANDLE hFile,
        _In_ FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
        _Out_writes_bytes_(dwBufferSize) LPVOID lpFileInformation,
        _In_ DWORD dwBufferSize);
  } // namespace f
} // namespace mingw_thunk
