#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 CreateHardLinkW,
                 _In_ LPCWSTR lpFileName,
                 _In_ LPCWSTR lpExistingFileName,
                 _Reserved_ LPSECURITY_ATTRIBUTES lpSecurityAttributes)
  {
    if (const auto pfn = try_get_CreateHardLinkW())
      return pfn(lpFileName, lpExistingFileName, lpSecurityAttributes);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
} // namespace mingw_thunk
