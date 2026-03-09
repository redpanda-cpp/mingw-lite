#include "ConvertStringSecurityDescriptorToSecurityDescriptorW.h"

#include <thunk/_common.h>

#include <sddl.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(advapi32,
                 16,
                 BOOL,
                 WINAPI,
                 ConvertStringSecurityDescriptorToSecurityDescriptorW,
                 _In_ LPCWSTR StringSecurityDescriptor,
                 _In_ DWORD StringSDRevision,
                 _Out_ PSECURITY_DESCRIPTOR *SecurityDescriptor,
                 _Out_ PULONG SecurityDescriptorSize)
  {
    __DISPATCH_THUNK_2(
        ConvertStringSecurityDescriptorToSecurityDescriptorW,
        const auto pfn =
            try_get_ConvertStringSecurityDescriptorToSecurityDescriptorW(),
        pfn,
        &f::fallback_ConvertStringSecurityDescriptorToSecurityDescriptorW);

    return dllimport_ConvertStringSecurityDescriptorToSecurityDescriptorW(
        StringSecurityDescriptor,
        StringSDRevision,
        SecurityDescriptor,
        SecurityDescriptorSize);
  }

  namespace f
  {
    BOOL __stdcall
    fallback_ConvertStringSecurityDescriptorToSecurityDescriptorW(
        _In_ LPCWSTR StringSecurityDescriptor,
        _In_ DWORD StringSDRevision,
        _Out_ PSECURITY_DESCRIPTOR *SecurityDescriptor,
        _Out_ PULONG SecurityDescriptorSize)
    {
      *SecurityDescriptor = nullptr;
      *SecurityDescriptorSize = 0;
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
