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
    if (internal::os_geq(VER_PLATFORM_WIN32_WINDOWS, 4, 10))
      return __ms_MultiByteToWideChar(CodePage,
                                      dwFlags,
                                      lpMultiByteStr,
                                      cbMultiByte,
                                      lpWideCharStr,
                                      cchWideChar);

    return impl::win95_MultiByteToWideChar(CodePage,
                                           dwFlags,
                                           lpMultiByteStr,
                                           cbMultiByte,
                                           lpWideCharStr,
                                           cchWideChar);
  }
} // namespace mingw_thunk
