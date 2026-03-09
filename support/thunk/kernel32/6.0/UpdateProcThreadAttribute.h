#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_UpdateProcThreadAttribute(
        _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
        _In_ DWORD dwFlags,
        _In_ DWORD_PTR Attribute,
        _In_ PVOID lpValue,
        _In_ SIZE_T cbSize,
        _Out_opt_ PVOID lpPreviousValue,
        _In_opt_ PSIZE_T lpReturnSize);
  }
} // namespace mingw_thunk
