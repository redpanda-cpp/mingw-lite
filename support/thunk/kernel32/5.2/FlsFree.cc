#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 4, BOOL, WINAPI, FlsFree, _In_ DWORD dwFlsIndex)
  {
    if (const auto pfn = try_get_FlsFree())
      return pfn(dwFlsIndex);

    return TlsFree(dwFlsIndex);
  }
} // namespace mingw_thunk
