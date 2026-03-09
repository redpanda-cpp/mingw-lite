#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_SetHandleInformation(_In_ HANDLE hObject,
                                              _In_ DWORD dwMask,
                                              _In_ DWORD dwFlags);
  } // namespace f
} // namespace mingw_thunk
