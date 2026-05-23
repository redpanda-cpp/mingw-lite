#include "OpenThread.h"
#include "thunk/os.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>

#include <windows.h>
#include <winternl.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 HANDLE,
                 WINAPI,
                 OpenThread,
                 _In_ DWORD dwDesiredAccess,
                 _In_ BOOL bInheritHandle,
                 _In_ DWORD dwThreadId)
  {
#if THUNK_LEVEL >= NTDDI_WIN4
    __DISPATCH_THUNK_2(OpenThread,
                       const auto pfn = try_get_OpenThread(),
                       pfn,
                       &f::winnt_OpenThread);
#else
    __DISPATCH_THUNK_3(OpenThread,
                       const auto pfn = try_get_OpenThread(),
                       pfn,
                       i::is_nt(),
                       &f::winnt_OpenThread,
                       &f::win9x_OpenThread);
#endif

    return dllimport_OpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
  }

  namespace f
  {
    HANDLE WINAPI winnt_OpenThread(_In_ DWORD dwDesiredAccess,
                                   _In_ BOOL bInheritHandle,
                                   _In_ DWORD dwThreadId)
    {
#if THUNK_LEVEL >= NTDDI_WIN4
      const auto NtOpenThread = __ms_NtOpenThread;
#else
      const auto NtOpenThread = ntdll_NtOpenThread();
#endif

      HANDLE hThread;
      CLIENT_ID cid = {};
      cid.UniqueThread = (HANDLE)(uintptr_t)dwThreadId;
      OBJECT_ATTRIBUTES oa = {};
      if (bInheritHandle)
        oa.Attributes = OBJ_INHERIT;
      if (NtOpenThread(&hThread, dwDesiredAccess, &oa, &cid) < 0) {
        SetLastError(ERROR_INVALID_HANDLE);
        return NULL;
      }
      return hThread;
    }

    HANDLE WINAPI win9x_OpenThread(_In_ DWORD dwDesiredAccess,
                                   _In_ BOOL bInheritHandle,
                                   _In_ DWORD dwThreadId)
    {
      constexpr int K32OBJ_PROCESS = 6;
      constexpr int K32OBJ_THREAD = 7;

      uintptr_t tdb = dwThreadId ^ i::win9x_obfuscator();
      char &type = *(char *)tdb;
      if (type != K32OBJ_THREAD) {
        SetLastError(ERROR_INVALID_HANDLE);
        return NULL;
      }

      type = K32OBJ_PROCESS;
      HANDLE h = OpenProcess(dwDesiredAccess, bInheritHandle, dwThreadId);
      type = K32OBJ_THREAD;
      return h;
    }
  } // namespace f
} // namespace mingw_thunk
