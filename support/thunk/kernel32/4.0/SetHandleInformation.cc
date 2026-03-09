#include "SetHandleInformation.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 SetHandleInformation,
                 _In_ HANDLE hObject,
                 _In_ DWORD dwMask,
                 _In_ DWORD dwFlags)
  {
    __DISPATCH_THUNK_2(SetHandleInformation,
                       i::is_nt(),
                       &__ms_SetHandleInformation,
                       &f::win9x_SetHandleInformation);

    return dllimport_SetHandleInformation(hObject, dwMask, dwFlags);
  }

  namespace f
  {
    BOOL __stdcall win9x_SetHandleInformation(_In_ HANDLE hObject,
                                              _In_ DWORD dwMask,
                                              _In_ DWORD dwFlags)
    {
      if (!hObject) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
      }
      // required by Python
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
