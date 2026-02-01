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
    if (auto pfn = try_get_RemoveDllDirectory())
      return pfn(Cookie);

    // This should not be called because `AddDllDirectory` always fails.
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
  }
} // namespace mingw_thunk
