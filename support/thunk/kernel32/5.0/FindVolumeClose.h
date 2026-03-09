#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_FindVolumeClose(_In_ HANDLE hFindVolume);
  } // namespace f
} // namespace mingw_thunk
