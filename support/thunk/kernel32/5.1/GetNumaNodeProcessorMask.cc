#include "GetNumaNodeProcessorMask.h"

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
    __DISPATCH_THUNK_2(GetNumaNodeProcessorMask,
                       const auto pfn = try_get_GetNumaNodeProcessorMask(),
                       pfn,
                       &f::fallback_GetNumaNodeProcessorMask);

    return dllimport_GetNumaNodeProcessorMask(Node, ProcessorMask);
  }

  namespace f
  {
    BOOL __stdcall
    fallback_GetNumaNodeProcessorMask(_In_ UCHAR Node,
                                      _Out_ PULONGLONG ProcessorMask)
    {
      if (Node == 0) {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        *ProcessorMask = si.dwActiveProcessorMask;
      } else {
        *ProcessorMask = 0;
      }
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
