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
    if (internal::is_nt())
      return __ms_SetHandleInformation(hObject, dwMask, dwFlags);

    if (!hObject) {
      SetLastError(ERROR_INVALID_HANDLE);
      return FALSE;
    }
    // required by Python
    return TRUE;
  }
} // namespace mingw_thunk
