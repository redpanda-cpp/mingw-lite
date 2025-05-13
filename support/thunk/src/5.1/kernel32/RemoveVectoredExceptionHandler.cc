#include <thunk/_common.h>

#include <windows.h>
#include <winerror.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, ULONG, WINAPI, RemoveVectoredExceptionHandler, PVOID Handle)
  {
    if (const auto pfn = try_get_RemoveVectoredExceptionHandler())
      return pfn(Handle);

    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
  }
} // namespace mingw_thunk
