#include "GetHandleInformation.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 GetHandleInformation,
                 _In_ HANDLE hObject,
                 _Out_ LPDWORD lpdwFlags)
  {
    __DISPATCH_THUNK_2(GetHandleInformation,
                       i::is_nt(),
                       &__ms_GetHandleInformation,
                       &f::win9x_GetHandleInformation);

    return dllimport_GetHandleInformation(hObject, lpdwFlags);
  }

  namespace f
  {
    BOOL __stdcall win9x_GetHandleInformation(_In_ HANDLE hObject,
                                              _Out_ LPDWORD lpdwFlags)
    {
      if (!hObject) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
      }
      *lpdwFlags = 0;
      // required by winpthreads
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
