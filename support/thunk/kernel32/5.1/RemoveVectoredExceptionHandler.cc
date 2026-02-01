#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, ULONG, WINAPI, RemoveVectoredExceptionHandler, PVOID Handle)
  {
    if (const auto pfn = try_get_RemoveVectoredExceptionHandler())
      return pfn(Handle);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
  }
} // namespace mingw_thunk
