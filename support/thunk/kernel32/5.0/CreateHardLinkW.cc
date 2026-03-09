#include "CreateHardLinkW.h"

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
    __DISPATCH_THUNK_2(CreateHardLinkW,
                       const auto pfn = try_get_CreateHardLinkW(),
                       pfn,
                       &f::fallback_CreateHardLinkW);

    return dllimport_CreateHardLinkW(
        lpFileName, lpExistingFileName, lpSecurityAttributes);
  }

  namespace f
  {
    BOOL __stdcall fallback_CreateHardLinkW(_In_ LPCWSTR lpFileName,
                                            _In_ LPCWSTR lpExistingFileName,
                                            _Reserved_ LPSECURITY_ATTRIBUTES
                                                lpSecurityAttributes)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
