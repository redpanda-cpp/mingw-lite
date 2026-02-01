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
    if (internal::is_nt())
      return __ms_GetHandleInformation(hObject, lpdwFlags);

    if (!hObject) {
      SetLastError(ERROR_INVALID_HANDLE);
      return FALSE;
    }
    *lpdwFlags = 0;
    // required by winpthreads
    return TRUE;
  }
} // namespace mingw_thunk
