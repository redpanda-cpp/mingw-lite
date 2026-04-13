#include "GetUserGeoID.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, GEOID, WINAPI, GetUserGeoID, _In_ GEOCLASS GeoClass)
  {
    __DISPATCH_THUNK_2(GetUserGeoID,
                       const auto pfn = try_get_GetUserGeoID(),
                       pfn,
                       &f::fallback_GetUserGeoID);

    return dllimport_GetUserGeoID(GeoClass);
  }

  namespace f
  {
    GEOID __stdcall fallback_GetUserGeoID(_In_ GEOCLASS GeoClass)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return GEOID_NOT_AVAILABLE;
    }
  } // namespace f
} // namespace mingw_thunk
