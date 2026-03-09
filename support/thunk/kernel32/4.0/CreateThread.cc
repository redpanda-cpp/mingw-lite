#include "CreateThread.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 24,
                 HANDLE,
                 WINAPI,
                 CreateThread,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
                 _In_ SIZE_T dwStackSize,
                 _In_ LPTHREAD_START_ROUTINE lpStartAddress,
                 _In_opt_ LPVOID lpParameter,
                 _In_ DWORD dwCreationFlags,
                 _Out_opt_ LPDWORD lpThreadId)
  {
    __DISPATCH_THUNK_2(
        CreateThread, i::is_nt(), &__ms_CreateThread, &f::win9x_CreateThread);

    return dllimport_CreateThread(lpThreadAttributes,
                                  dwStackSize,
                                  lpStartAddress,
                                  lpParameter,
                                  dwCreationFlags,
                                  lpThreadId);
  }

  namespace f
  {
    HANDLE __stdcall
    win9x_CreateThread(_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
                       _In_ SIZE_T dwStackSize,
                       _In_ LPTHREAD_START_ROUTINE lpStartAddress,
                       _In_opt_ LPVOID lpParameter,
                       _In_ DWORD dwCreationFlags,
                       _Out_opt_ LPDWORD lpThreadId)
    {
      // 9x: tid is required.
      DWORD tid = 0;
      return __ms_CreateThread(lpThreadAttributes,
                               dwStackSize,
                               lpStartAddress,
                               lpParameter,
                               dwCreationFlags,
                               lpThreadId ? lpThreadId : &tid);
    }
  } // namespace f
} // namespace mingw_thunk
