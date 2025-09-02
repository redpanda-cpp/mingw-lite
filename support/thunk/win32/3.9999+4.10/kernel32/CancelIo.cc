#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 4, BOOL, WINAPI, CancelIo, _In_ HANDLE hFile)
  {
    if (const auto pfn = try_get_CancelIo())
      return pfn(hFile);

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
  }
} // namespace mingw_thunk
