#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    win9x_CopyFileExW(_In_ LPCWSTR lpExistingFileName,
                      _In_ LPCWSTR lpNewFileName,
                      _In_opt_ LPPROGRESS_ROUTINE lpProgressRoutine,
                      _In_opt_ LPVOID lpData,
                      _In_opt_ LPBOOL pbCancel,
                      _In_ DWORD dwCopyFlags);
  }
} // namespace mingw_thunk
