#include "RemoveVectoredExceptionHandler.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, ULONG, WINAPI, RemoveVectoredExceptionHandler, PVOID Handle)
  {
    __DISPATCH_THUNK_2(RemoveVectoredExceptionHandler,
                       const auto pfn =
                           try_get_RemoveVectoredExceptionHandler(),
                       pfn,
                       &f::fallback_RemoveVectoredExceptionHandler);

    return dllimport_RemoveVectoredExceptionHandler(Handle);
  }

  namespace f
  {
    ULONG __stdcall fallback_RemoveVectoredExceptionHandler(PVOID Handle)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
