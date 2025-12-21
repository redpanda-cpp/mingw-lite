#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 40,
                 BOOL,
                 WINAPI,
                 CreateProcessA,
                 _In_opt_ LPCSTR lpApplicationName,
                 _Inout_opt_ LPSTR lpCommandLine,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
                 _In_ BOOL bInheritHandles,
                 _In_ DWORD dwCreationFlags,
                 _In_opt_ LPVOID lpEnvironment,
                 _In_opt_ LPCSTR lpCurrentDirectory,
                 _In_ LPSTARTUPINFOA lpStartupInfo,
                 _Out_ LPPROCESS_INFORMATION lpProcessInformation)
  {
    stl::wstring w_app_name;
    if (lpApplicationName)
      w_app_name = internal::u2w(lpApplicationName);
    stl::wstring w_cmd_line;
    if (lpCommandLine)
      w_cmd_line = internal::u2w(lpCommandLine);

    stl::wstring w_env;
    if (lpEnvironment && !(dwCreationFlags & CREATE_UNICODE_ENVIRONMENT)) {
      size_t block_size = 0;
      const char *u_env = (const char *)lpEnvironment;
      while (u_env[block_size]) {
        size_t l = strlen(u_env + block_size);
        block_size += l + 1;
      }
      // block terminator is implicitly added
      w_env = internal::u2w(u_env, block_size);
    }

    stl::wstring w_cwd;
    if (lpCurrentDirectory)
      w_cwd = internal::u2w(lpCurrentDirectory);

    stl::wstring w_desktop;
    if (lpStartupInfo->lpDesktop)
      w_desktop = internal::u2w(lpStartupInfo->lpDesktop);
    stl::wstring w_title;
    if (lpStartupInfo->lpTitle)
      w_title = internal::u2w(lpStartupInfo->lpTitle);
    STARTUPINFOEXW w_startup_info{
        .StartupInfo =
            {
                sizeof(STARTUPINFOW),
                nullptr,
                lpStartupInfo->lpDesktop ? w_desktop.data() : nullptr,
                lpStartupInfo->lpTitle ? w_title.data() : nullptr,
                lpStartupInfo->dwX,
                lpStartupInfo->dwY,
                lpStartupInfo->dwXSize,
                lpStartupInfo->dwYSize,
                lpStartupInfo->dwXCountChars,
                lpStartupInfo->dwYCountChars,
                lpStartupInfo->dwFillAttribute,
                lpStartupInfo->dwFlags,
                lpStartupInfo->wShowWindow,
                0,
                nullptr,
                lpStartupInfo->hStdInput,
                lpStartupInfo->hStdOutput,
                lpStartupInfo->hStdError,
            },
        .lpAttributeList = nullptr,
    };
    if (dwCreationFlags & EXTENDED_STARTUPINFO_PRESENT) {
      w_startup_info.StartupInfo.cb = sizeof(STARTUPINFOEXW);
      w_startup_info.lpAttributeList =
          ((STARTUPINFOEXA *)lpStartupInfo)->lpAttributeList;
    };

    return CreateProcessW(lpApplicationName ? w_app_name.c_str() : nullptr,
                          lpCommandLine ? w_cmd_line.data() : nullptr,
                          lpProcessAttributes,
                          lpThreadAttributes,
                          bInheritHandles,
                          dwCreationFlags | CREATE_UNICODE_ENVIRONMENT,
                          lpEnvironment
                              ? (dwCreationFlags & CREATE_UNICODE_ENVIRONMENT
                                     ? lpEnvironment
                                     : w_env.data())
                              : nullptr,
                          lpCurrentDirectory ? w_cwd.c_str() : nullptr,
                          (STARTUPINFOW *)&w_startup_info,
                          lpProcessInformation);
  }
} // namespace mingw_thunk
