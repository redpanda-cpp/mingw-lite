#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 GetNumaNodeProcessorMask,
                 _In_ UCHAR Node,
                 _Out_ PULONGLONG ProcessorMask)
  {
    if (const auto pfn = try_get_GetNumaNodeProcessorMask())
      return pfn(Node, ProcessorMask);

    if (Node == 0) {
      SYSTEM_INFO si;
      GetSystemInfo(&si);
      *ProcessorMask = si.dwActiveProcessorMask;
    } else {
      *ProcessorMask = 0;
    }
    return TRUE;
  }
} // namespace mingw_thunk
