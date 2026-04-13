#include "GetGeoInfoW.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 20,
                 int,
                 WINAPI,
                 GetGeoInfoW,
                 _In_ GEOID Location,
                 _In_ GEOTYPE GeoType,
                 _Out_opt_ LPWSTR lpGeoData,
                 _In_ int cchData,
                 _In_ LANGID LangId)
  {
    __DISPATCH_THUNK_2(GetGeoInfoW,
                       const auto pfn = try_get_GetGeoInfoW(),
                       pfn,
                       &f::fallback_GetGeoInfoW);

    return dllimport_GetGeoInfoW(Location, GeoType, lpGeoData, cchData, LangId);
  }

  namespace f
  {
    int __stdcall fallback_GetGeoInfoW(_In_ GEOID Location,
                                       _In_ GEOTYPE GeoType,
                                       _Out_opt_ LPWSTR lpGeoData,
                                       _In_ int cchData,
                                       _In_ LANGID LangId)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
