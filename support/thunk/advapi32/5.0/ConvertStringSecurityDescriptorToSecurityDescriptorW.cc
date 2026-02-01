#include <thunk/_common.h>

#include <sddl.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 16,
                 BOOL,
                 WINAPI,
                 ConvertStringSecurityDescriptorToSecurityDescriptorW,
                 _In_ LPCWSTR StringSecurityDescriptor,
                 _In_ DWORD StringSDRevision,
                 _Out_ PSECURITY_DESCRIPTOR *SecurityDescriptor,
                 _Out_ PULONG SecurityDescriptorSize)
  {
    if (const auto pfn =
            try_get_ConvertStringSecurityDescriptorToSecurityDescriptorW())
      return pfn(StringSecurityDescriptor,
                 StringSDRevision,
                 SecurityDescriptor,
                 SecurityDescriptorSize);

    *SecurityDescriptor = nullptr;
    *SecurityDescriptorSize = 0;
    return TRUE;
  }
} // namespace mingw_thunk
