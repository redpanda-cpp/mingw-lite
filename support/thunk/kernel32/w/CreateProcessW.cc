#include "CreateProcessW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 40,
                 BOOL,
                 WINAPI,
                 CreateProcessW,
                 _In_opt_ LPCWSTR lpApplicationName,
                 _Inout_opt_ LPWSTR lpCommandLine,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
                 _In_ BOOL bInheritHandles,
                 _In_ DWORD dwCreationFlags,
                 _In_opt_ LPVOID lpEnvironment,
                 _In_opt_ LPCWSTR lpCurrentDirectory,
                 _In_ LPSTARTUPINFOW lpStartupInfo,
                 _Out_ LPPROCESS_INFORMATION lpProcessInformation)
  {
    __DISPATCH_THUNK_2(CreateProcessW,
                       i::is_nt(),
                       &__ms_CreateProcessW,
                       &f::win9x_CreateProcessW);

    return dllimport_CreateProcessW(lpApplicationName,
                                    lpCommandLine,
                                    lpProcessAttributes,
                                    lpThreadAttributes,
                                    bInheritHandles,
                                    dwCreationFlags,
                                    lpEnvironment,
                                    lpCurrentDirectory,
                                    lpStartupInfo,
                                    lpProcessInformation);
  }

  namespace f
  {
    BOOL __stdcall
    win9x_CreateProcessW(_In_opt_ LPCWSTR lpApplicationName,
                         _Inout_opt_ LPWSTR lpCommandLine,
                         _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
                         _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
                         _In_ BOOL bInheritHandles,
                         _In_ DWORD dwCreationFlags,
                         _In_opt_ LPVOID lpEnvironment,
                         _In_opt_ LPCWSTR lpCurrentDirectory,
                         _In_ LPSTARTUPINFOW lpStartupInfo,
                         _Out_ LPPROCESS_INFORMATION lpProcessInformation)
    {
      if (!lpStartupInfo) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      d::a_str a_app_name;
      if (lpApplicationName && !a_app_name.from_w(lpApplicationName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      d::a_str a_cmd_line;
      if (lpCommandLine && !a_cmd_line.from_w(lpCommandLine)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      d::a_str a_env;
      if (lpEnvironment && (dwCreationFlags & CREATE_UNICODE_ENVIRONMENT)) {
        size_t block_size = 0;
        const wchar_t *w_env = (const wchar_t *)lpEnvironment;
        while (w_env[block_size]) {
          size_t l = c::wcslen(w_env + block_size);
          block_size += l + 1;
        }
        // block terminator is implicitly added
        if (!a_env.from_w(w_env, block_size)) {
          SetLastError(ERROR_OUTOFMEMORY);
          return FALSE;
        }
      }

      d::a_str a_cwd;
      if (lpCurrentDirectory && !a_cwd.from_w(lpCurrentDirectory)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      d::a_str a_desktop;
      if (lpStartupInfo->lpDesktop &&
          !a_desktop.from_w(lpStartupInfo->lpDesktop)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      d::a_str a_title;
      if (lpStartupInfo->lpTitle && !a_title.from_w(lpStartupInfo->lpTitle)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
      }

      STARTUPINFOA a_startup_info{
          .cb = sizeof(STARTUPINFOA),
          .lpReserved = nullptr,
          .lpDesktop = lpStartupInfo->lpDesktop ? a_desktop.data() : nullptr,
          .lpTitle = lpStartupInfo->lpTitle ? a_title.data() : nullptr,
          .dwX = lpStartupInfo->dwX,
          .dwY = lpStartupInfo->dwY,
          .dwXSize = lpStartupInfo->dwXSize,
          .dwYSize = lpStartupInfo->dwYSize,
          .dwXCountChars = lpStartupInfo->dwXCountChars,
          .dwYCountChars = lpStartupInfo->dwYCountChars,
          .dwFillAttribute = lpStartupInfo->dwFillAttribute,
          .dwFlags = lpStartupInfo->dwFlags,
          .wShowWindow = lpStartupInfo->wShowWindow,
          .cbReserved2 = 0,
          .lpReserved2 = nullptr,
          .hStdInput = lpStartupInfo->hStdInput,
          .hStdOutput = lpStartupInfo->hStdOutput,
          .hStdError = lpStartupInfo->hStdError,
      };

      return __ms_CreateProcessA(
          lpApplicationName ? a_app_name.data() : nullptr,
          lpCommandLine ? a_cmd_line.data() : nullptr,
          lpProcessAttributes,
          lpThreadAttributes,
          bInheritHandles,
          dwCreationFlags & ~CREATE_UNICODE_ENVIRONMENT,
          lpEnvironment
              ? (dwCreationFlags & CREATE_UNICODE_ENVIRONMENT ? a_env.data()
                                                              : lpEnvironment)
              : nullptr,
          lpCurrentDirectory ? a_cwd.data() : nullptr,
          &a_startup_info,
          lpProcessInformation);
    }
  } // namespace f
} // namespace mingw_thunk
