#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE __stdcall
    win9x_CreateThread(_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
                       _In_ SIZE_T dwStackSize,
                       _In_ LPTHREAD_START_ROUTINE lpStartAddress,
                       _In_opt_ LPVOID lpParameter,
                       _In_ DWORD dwCreationFlags,
                       _Out_opt_ LPDWORD lpThreadId);
  } // namespace f
} // namespace mingw_thunk
