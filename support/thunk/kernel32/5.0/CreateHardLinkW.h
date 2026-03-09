#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_CreateHardLinkW(_In_ LPCWSTR lpFileName,
                                            _In_ LPCWSTR lpExistingFileName,
                                            _Reserved_ LPSECURITY_ATTRIBUTES
                                                lpSecurityAttributes);
  } // namespace f
} // namespace mingw_thunk
