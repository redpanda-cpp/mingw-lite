#include "AddDllDirectory.h"

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
    __DISPATCH_THUNK_2(AddDllDirectory,
                       const auto pfn = try_get_AddDllDirectory(),
                       pfn,
                       &f::fallback_AddDllDirectory);

    return dllimport_AddDllDirectory(NewDirectory);
  }

  namespace f
  {
    DLL_DIRECTORY_COOKIE __stdcall
    fallback_AddDllDirectory(_In_ PCWSTR NewDirectory)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return nullptr;
    }
  } // namespace f
} // namespace mingw_thunk
