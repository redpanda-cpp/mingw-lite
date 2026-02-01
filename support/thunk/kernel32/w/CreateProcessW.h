#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    BOOL
    win9x_CreateProcessW(_In_opt_ LPCWSTR lpApplicationName,
                         _Inout_opt_ LPWSTR lpCommandLine,
                         _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
                         _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
                         _In_ BOOL bInheritHandles,
                         _In_ DWORD dwCreationFlags,
                         _In_opt_ LPVOID lpEnvironment,
                         _In_opt_ LPCWSTR lpCurrentDirectory,
                         _In_ LPSTARTUPINFOW lpStartupInfo,
                         _Out_ LPPROCESS_INFORMATION lpProcessInformation);
  }
} // namespace mingw_thunk
