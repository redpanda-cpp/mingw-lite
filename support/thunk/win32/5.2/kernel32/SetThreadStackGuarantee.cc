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
    if (const auto pfn = try_get_SetThreadStackGuarantee())
      return pfn(StackSizeInBytes);

    if (*StackSizeInBytes == 0) {
      *StackSizeInBytes = 0x1000; // 4 KiB page
      return TRUE;
    } else {
      SetLastError(ERROR_NOT_SUPPORTED);
      return FALSE;
    }
  }
} // namespace mingw_thunk
