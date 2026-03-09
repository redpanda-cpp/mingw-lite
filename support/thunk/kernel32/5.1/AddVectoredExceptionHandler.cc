#include "AddVectoredExceptionHandler.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 PVOID,
                 WINAPI,
                 AddVectoredExceptionHandler,
                 ULONG First,
                 PVECTORED_EXCEPTION_HANDLER Handler)
  {
    __DISPATCH_THUNK_2(AddVectoredExceptionHandler,
                       const auto pfn = try_get_AddVectoredExceptionHandler(),
                       pfn,
                       &f::fallback_AddVectoredExceptionHandler);

    return dllimport_AddVectoredExceptionHandler(First, Handler);
  }

  namespace f
  {
    PVOID __stdcall
    fallback_AddVectoredExceptionHandler(ULONG First,
                                         PVECTORED_EXCEPTION_HANDLER Handler)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return NULL;
    }
  } // namespace f
} // namespace mingw_thunk
