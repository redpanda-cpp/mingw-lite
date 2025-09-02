#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 DWORD,
                 WINAPI,
                 FlsAlloc,
                 _In_opt_ PFLS_CALLBACK_FUNCTION lpCallback)
  {
    if (const auto pfn = try_get_FlsAlloc())
      return pfn(lpCallback);

    // UCRT ignores the callback function, so do we.
    return TlsAlloc();
  }
} // namespace mingw_thunk
