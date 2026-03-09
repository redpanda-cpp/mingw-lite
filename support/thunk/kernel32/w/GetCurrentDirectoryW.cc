#include "GetCurrentDirectoryW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 8,
                 DWORD,
                 WINAPI,
                 GetCurrentDirectoryW,
                 _In_ DWORD nBufferLength,
                 _Out_ LPWSTR lpBuffer)
  {
    __DISPATCH_THUNK_2(GetCurrentDirectoryW,
                       i::is_nt(),
                       &__ms_GetCurrentDirectoryW,
                       &f::win9x_GetCurrentDirectoryW);

    return dllimport_GetCurrentDirectoryW(nBufferLength, lpBuffer);
  }

  namespace f
  {
    DWORD __stdcall win9x_GetCurrentDirectoryW(_In_ DWORD nBufferLength,
                                               _Out_ LPWSTR lpBuffer)
    {
      // dry run for buffer size
      if (nBufferLength && !lpBuffer) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }

      d::a_str a_filename{d::max_path_tag{}};
      DWORD a_size = __ms_GetCurrentDirectoryA(MAX_PATH, a_filename.data());
      if (a_size == 0)
        return 0;

      if (a_size >= MAX_PATH) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return 0;
      }

      d::w_str w_filename;
      if (!w_filename.from_a(a_filename.c_str(), a_size)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      if (nBufferLength >= w_filename.size() + 1) {
        c::wmemcpy(lpBuffer, w_filename.c_str(), w_filename.size());
        lpBuffer[w_filename.size()] = L'\0';
        return w_filename.size();
      } else {
        return w_filename.size() + 1;
      }
    }
  } // namespace f
} // namespace mingw_thunk
