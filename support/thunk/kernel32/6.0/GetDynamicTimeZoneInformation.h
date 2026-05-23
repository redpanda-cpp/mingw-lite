#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall fallback_GetDynamicTimeZoneInformation(
        _Out_ PDYNAMIC_TIME_ZONE_INFORMATION pTimeZoneInformation);
  }
} // namespace mingw_thunk
