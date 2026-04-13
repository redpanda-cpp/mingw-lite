#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    GEOID __stdcall fallback_GetUserGeoID(_In_ GEOCLASS GeoClass);
  }
} // namespace mingw_thunk
