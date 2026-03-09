#include "MultiByteToWideChar.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 24,
                 int,
                 WINAPI,
                 MultiByteToWideChar,
                 _In_ UINT CodePage,
                 _In_ DWORD dwFlags,
                 _In_ LPCSTR lpMultiByteStr,
                 _In_ int cbMultiByte,
                 _Out_opt_ LPWSTR lpWideCharStr,
                 _In_ int cchWideChar)
  {
    __DISPATCH_THUNK_2(MultiByteToWideChar,
                       i::os_version() >= g::win32_win98,
                       &__ms_MultiByteToWideChar,
                       &f::win95_MultiByteToWideChar);

    return dllimport_MultiByteToWideChar(CodePage,
                                         dwFlags,
                                         lpMultiByteStr,
                                         cbMultiByte,
                                         lpWideCharStr,
                                         cchWideChar);
  }
} // namespace mingw_thunk
