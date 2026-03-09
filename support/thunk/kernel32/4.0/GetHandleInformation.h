#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_GetHandleInformation(_In_ HANDLE hObject,
                                              _Out_ LPDWORD lpdwFlags);
  } // namespace f
} // namespace mingw_thunk
