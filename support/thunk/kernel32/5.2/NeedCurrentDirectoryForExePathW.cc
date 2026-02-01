#include <thunk/_common.h>

#include <windows.h>

#include <nocrt/wchar.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 NeedCurrentDirectoryForExePathW,
                 _In_ LPCWSTR ExeName)
  {
    if (const auto pfn = try_get_NeedCurrentDirectoryForExePathW())
      return pfn(ExeName);

    if (libc::wcschr(ExeName, L'\\'))
      return TRUE;

    wchar_t value[2];
    return !GetEnvironmentVariableW(
        L"NoDefaultCurrentDirectoryInExePath", value, 2);
  }
} // namespace mingw_thunk
