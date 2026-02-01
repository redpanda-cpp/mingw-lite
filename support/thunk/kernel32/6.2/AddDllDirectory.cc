#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 DLL_DIRECTORY_COOKIE,
                 WINAPI,
                 AddDllDirectory,
                 _In_ PCWSTR NewDirectory)
  {
    if (auto pfn = try_get_AddDllDirectory())
      return pfn(NewDirectory);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return nullptr;
  }
} // namespace mingw_thunk
