#include "../a/3.9999+4.10/WideCharToMultiByte.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 32,
                 int,
                 WINAPI,
                 WideCharToMultiByte,
                 _In_ UINT CodePage,
                 _In_ DWORD dwFlags,
                 _In_ LPCWCH lpWideCharStr,
                 _In_ int cchWideChar,
                 _Out_opt_ LPSTR lpMultiByteStr,
                 _In_ int cbMultiByte,
                 _In_opt_ LPCCH lpDefaultChar,
                 _In_opt_ LPBOOL lpUsedDefaultChar)
  {
    if (CodePage == CP_ACP || CodePage == CP_OEMCP) {
      CodePage = CP_UTF8;

      lpDefaultChar = nullptr;
      lpUsedDefaultChar = nullptr;

      // unset all flags except WC_ERR_INVALID_CHARS
      dwFlags &= WC_ERR_INVALID_CHARS;
    }

#if THUNK_LEVEL >= NTDDI_WIN98
    return __ms_WideCharToMultiByte(CodePage,
                                    dwFlags,
                                    lpWideCharStr,
                                    cchWideChar,
                                    lpMultiByteStr,
                                    cbMultiByte,
                                    lpDefaultChar,
                                    lpUsedDefaultChar);
#else
    if (internal::os_geq(VER_PLATFORM_WIN32_WINDOWS, 4, 10))
      return __ms_WideCharToMultiByte(CodePage,
                                      dwFlags,
                                      lpWideCharStr,
                                      cchWideChar,
                                      lpMultiByteStr,
                                      cbMultiByte,
                                      lpDefaultChar,
                                      lpUsedDefaultChar);
    return impl::win95_WideCharToMultiByte(CodePage,
                                           dwFlags,
                                           lpWideCharStr,
                                           cchWideChar,
                                           lpMultiByteStr,
                                           cbMultiByte,
                                           lpDefaultChar,
                                           lpUsedDefaultChar);
#endif
  }
} // namespace mingw_thunk
