#include "RemoveDllDirectory.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 RemoveDllDirectory,
                 _In_ DLL_DIRECTORY_COOKIE Cookie)
  {
    __DISPATCH_THUNK_2(RemoveDllDirectory,
                       const auto pfn = try_get_RemoveDllDirectory(),
                       pfn,
                       &f::fallback_RemoveDllDirectory);

    return dllimport_RemoveDllDirectory(Cookie);
  }

  namespace f
  {
    BOOL __stdcall fallback_RemoveDllDirectory(_In_ DLL_DIRECTORY_COOKIE Cookie)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
