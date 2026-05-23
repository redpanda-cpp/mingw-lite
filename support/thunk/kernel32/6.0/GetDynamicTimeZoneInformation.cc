#include "GetDynamicTimeZoneInformation.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 DWORD,
                 WINAPI,
                 GetDynamicTimeZoneInformation,
                 _Out_ PDYNAMIC_TIME_ZONE_INFORMATION pTimeZoneInformation)
  {
    __DISPATCH_THUNK_2(GetDynamicTimeZoneInformation,
                       const auto pfn = try_get_GetDynamicTimeZoneInformation(),
                       pfn,
                       &f::fallback_GetDynamicTimeZoneInformation);

    return dllimport_GetDynamicTimeZoneInformation(pTimeZoneInformation);
  }

  namespace f
  {
    DWORD __stdcall fallback_GetDynamicTimeZoneInformation(
        _Out_ PDYNAMIC_TIME_ZONE_INFORMATION pTimeZoneInformation)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return TIME_ZONE_ID_INVALID;
    }
  } // namespace f
} // namespace mingw_thunk
