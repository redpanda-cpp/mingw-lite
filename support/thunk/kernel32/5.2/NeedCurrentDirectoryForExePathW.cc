#include "NeedCurrentDirectoryForExePathW.h"

#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 NeedCurrentDirectoryForExePathW,
                 _In_ LPCWSTR ExeName)
  {
    __DISPATCH_THUNK_2(NeedCurrentDirectoryForExePathW,
                       const auto pfn =
                           try_get_NeedCurrentDirectoryForExePathW(),
                       pfn,
                       &f::fallback_NeedCurrentDirectoryForExePathW);

    return dllimport_NeedCurrentDirectoryForExePathW(ExeName);
  }

  namespace f
  {
    BOOL __stdcall
    fallback_NeedCurrentDirectoryForExePathW(_In_ LPCWSTR ExeName)
    {
      if (!ExeName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      if (c::wcschr(ExeName, L'\\'))
        return TRUE;

      wchar_t value[2];
      return !GetEnvironmentVariableW(
          L"NoDefaultCurrentDirectoryInExePath", value, 2);
    }
  } // namespace f
} // namespace mingw_thunk
