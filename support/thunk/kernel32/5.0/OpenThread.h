#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE WINAPI winnt_OpenThread(_In_ DWORD dwDesiredAccess,
                                   _In_ BOOL bInheritHandle,
                                   _In_ DWORD dwThreadId);

    HANDLE WINAPI win9x_OpenThread(_In_ DWORD dwDesiredAccess,
                                   _In_ BOOL bInheritHandle,
                                   _In_ DWORD dwThreadId);
  } // namespace f
} // namespace mingw_thunk
