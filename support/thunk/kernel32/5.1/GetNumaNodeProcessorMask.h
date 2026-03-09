#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_GetNumaNodeProcessorMask(_In_ UCHAR Node,
                                      _Out_ PULONGLONG ProcessorMask);
  }
} // namespace mingw_thunk
