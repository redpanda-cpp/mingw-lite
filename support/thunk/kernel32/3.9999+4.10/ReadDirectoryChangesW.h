#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win95_ReadDirectoryChangesW(
        _In_ HANDLE hDirectory,
        _Out_ LPVOID lpBuffer,
        _In_ DWORD nBufferLength,
        _In_ BOOL bWatchSubtree,
        _In_ DWORD dwNotifyFilter,
        _Out_opt_ LPDWORD lpBytesReturned,
        _Inout_opt_ LPOVERLAPPED lpOverlapped,
        _In_opt_ LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
  } // namespace f
} // namespace mingw_thunk
