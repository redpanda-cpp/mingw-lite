#include "SetThreadStackGuarantee.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 SetThreadStackGuarantee,
                 _Inout_ PULONG StackSizeInBytes)
  {
    __DISPATCH_THUNK_2(SetThreadStackGuarantee,
                       const auto pfn = try_get_SetThreadStackGuarantee(),
                       pfn,
                       &f::fallback_SetThreadStackGuarantee);

    return dllimport_SetThreadStackGuarantee(StackSizeInBytes);
  }

  namespace f
  {
    BOOL __stdcall
    fallback_SetThreadStackGuarantee(_Inout_ PULONG StackSizeInBytes)
    {
      if (*StackSizeInBytes == 0) {
        *StackSizeInBytes = 0x1000; // 4 KiB page
        return TRUE;
      } else {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
      }
    }
  } // namespace f
} // namespace mingw_thunk
