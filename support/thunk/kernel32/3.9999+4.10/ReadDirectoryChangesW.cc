#include "ReadDirectoryChangesW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 98 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 32,
                 BOOL,
                 WINAPI,
                 ReadDirectoryChangesW,
                 _In_ HANDLE hDirectory,
                 _Out_ LPVOID lpBuffer,
                 _In_ DWORD nBufferLength,
                 _In_ BOOL bWatchSubtree,
                 _In_ DWORD dwNotifyFilter,
                 _Out_opt_ LPDWORD lpBytesReturned,
                 _Inout_opt_ LPOVERLAPPED lpOverlapped,
                 _In_opt_ LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
  {
    // there's no ReadDirectoryChangesA
    // so we simply call the stub
    __DISPATCH_THUNK_2(ReadDirectoryChangesW,
                       const auto pfn = try_get_ReadDirectoryChangesW(),
                       pfn,
                       &f::win95_ReadDirectoryChangesW);

    return dllimport_ReadDirectoryChangesW(hDirectory,
                                           lpBuffer,
                                           nBufferLength,
                                           bWatchSubtree,
                                           dwNotifyFilter,
                                           lpBytesReturned,
                                           lpOverlapped,
                                           lpCompletionRoutine);
  }

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
        _In_opt_ LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
    {
      SetLastError(ERROR_NOT_SUPPORTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
