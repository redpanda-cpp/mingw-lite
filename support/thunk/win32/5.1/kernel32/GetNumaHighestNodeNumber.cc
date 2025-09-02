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
    if (const auto pfn = try_get_GetNumaHighestNodeNumber())
      return pfn(HighestNodeNumber);

    *HighestNodeNumber = 0;
    return TRUE;
  }
} // namespace mingw_thunk
