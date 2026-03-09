#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_ConvertStringSecurityDescriptorToSecurityDescriptorW(
        _In_ LPCWSTR StringSecurityDescriptor,
        _In_ DWORD StringSDRevision,
        _Out_ PSECURITY_DESCRIPTOR *SecurityDescriptor,
        _Out_ PULONG SecurityDescriptorSize);
  }
} // namespace mingw_thunk
