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
    if (const auto pfn = try_get_AddVectoredExceptionHandler())
      return pfn(First, Handler);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return NULL;
  }
} // namespace mingw_thunk
