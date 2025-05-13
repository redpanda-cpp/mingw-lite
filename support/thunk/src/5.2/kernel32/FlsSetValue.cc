#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 FlsSetValue,
                 _In_ DWORD dwFlsIndex,
                 _In_opt_ PVOID lpFlsData)
  {
    if (const auto pfn = try_get_FlsSetValue())
      return pfn(dwFlsIndex, lpFlsData);

    return TlsSetValue(dwFlsIndex, lpFlsData);
  }
} // namespace mingw_thunk
