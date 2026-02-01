#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
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
    if (internal::is_nt())
      return get_ReadDirectoryChangesW()(hDirectory,
                                         lpBuffer,
                                         nBufferLength,
                                         bWatchSubtree,
                                         dwNotifyFilter,
                                         lpBytesReturned,
                                         lpOverlapped,
                                         lpCompletionRoutine);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
  }
} // namespace mingw_thunk
