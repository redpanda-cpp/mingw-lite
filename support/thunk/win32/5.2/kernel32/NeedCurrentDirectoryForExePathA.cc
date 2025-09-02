#include <thunk/_common.h>

#include <windows.h>

#include <nocrt/string.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 NeedCurrentDirectoryForExePathA,
                 _In_ LPCSTR ExeName)
  {
    if (const auto pfn = try_get_NeedCurrentDirectoryForExePathA())
      return pfn(ExeName);

    // WONTFIX: DBCS can contain '\' in second byte.
    if (libc::strchr(ExeName, '\\'))
      return TRUE;

    char value[2];
    return !GetEnvironmentVariableA(
        "NoDefaultCurrentDirectoryInExePath", value, 2);
  }
} // namespace mingw_thunk
