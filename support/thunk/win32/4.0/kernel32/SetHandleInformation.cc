#include <thunk/_common.h>
#include <thunk/os.h>

#include <errhandlingapi.h>
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
      return get_SetHandleInformation()(hObject, dwMask, dwFlags);

    if (!hObject) {
      SetLastError(ERROR_INVALID_HANDLE);
      return FALSE;
    }
    // required by Python
    return TRUE;
  }
} // namespace mingw_thunk
