#include "GetNumaHighestNodeNumber.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 GetNumaHighestNodeNumber,
                 _Out_ PULONG HighestNodeNumber)
  {
    __DISPATCH_THUNK_2(GetNumaHighestNodeNumber,
                       const auto pfn = try_get_GetNumaHighestNodeNumber(),
                       pfn,
                       &f::fallback_GetNumaHighestNodeNumber);

    return dllimport_GetNumaHighestNodeNumber(HighestNodeNumber);
  }

  namespace f
  {
    BOOL __stdcall
    fallback_GetNumaHighestNodeNumber(_Out_ PULONG HighestNodeNumber)
    {
      *HighestNodeNumber = 0;
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
