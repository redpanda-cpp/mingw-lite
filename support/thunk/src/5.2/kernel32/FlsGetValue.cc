#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 4, PVOID, WINAPI, FlsGetValue, _In_ DWORD dwFlsIndex)
  {
    if (const auto pfn = try_get_FlsGetValue())
      return pfn(dwFlsIndex);

    return TlsGetValue(dwFlsIndex);
  }
} // namespace mingw_thunk
