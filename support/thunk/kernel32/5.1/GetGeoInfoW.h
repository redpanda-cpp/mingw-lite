#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    int __stdcall fallback_GetGeoInfoW(_In_ GEOID Location,
                                       _In_ GEOTYPE GeoType,
                                       _Out_opt_ LPWSTR lpGeoData,
                                       _In_ int cchData,
                                       _In_ LANGID LangId);
  }
} // namespace mingw_thunk
