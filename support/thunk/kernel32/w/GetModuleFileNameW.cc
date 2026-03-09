#include "GetModuleFileNameW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errhandlingapi.h>
#include <libloaderapi.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 12,
                 DWORD,
                 WINAPI,
                 GetModuleFileNameW,
                 _In_opt_ HMODULE hModule,
                 _Out_ LPWSTR lpFilename,
                 _In_ DWORD nSize)
  {
    __DISPATCH_THUNK_2(GetModuleFileNameW,
                       i::is_nt(),
                       &__ms_GetModuleFileNameW,
                       &f::win9x_GetModuleFileNameW);

    return dllimport_GetModuleFileNameW(hModule, lpFilename, nSize);
  }

  namespace f
  {
    DWORD __stdcall win9x_GetModuleFileNameW(_In_opt_ HMODULE hModule,
                                             _Out_ LPWSTR lpFilename,
                                             _In_ DWORD nSize)
    {
      // dry run for buffer size
      if (nSize && !lpFilename) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
      }

      d::a_str a_name{d::max_path_tag{}};
      DWORD ret = __ms_GetModuleFileNameA(hModule, a_name.data(), MAX_PATH);
      if (ret == 0)
        return 0;

      while (ret > a_name.size()) {
        if (!a_name.resize(ret)) {
          SetLastError(ERROR_OUTOFMEMORY);
          return 0;
        }
        ret = __ms_GetModuleFileNameA(hModule, a_name.data(), ret);
        if (ret == 0)
          return 0;
      }

      d::w_str w_name;
      if (!w_name.from_a(a_name.c_str(), ret)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }

      size_t w_size = w_name.size();
      if (nSize >= w_size + 1) {
        c::wmemcpy(lpFilename, w_name.c_str(), w_size);
        lpFilename[w_size] = 0;
        return w_size;
      } else {
        c::wmemcpy(lpFilename, w_name.c_str(), nSize - 1);
        lpFilename[nSize - 1] = 0;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return nSize;
      }
    }
  } // namespace f
} // namespace mingw_thunk
