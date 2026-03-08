#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 DWORD,
                 WINAPI,
                 GetModuleFileNameA,
                 _In_opt_ HMODULE hModule,
                 _Out_ LPSTR lpFilename,
                 _In_ DWORD nSize)
  {
    d::w_str w_buffer{d::max_path_tag{}};

    DWORD ret = GetModuleFileNameW(hModule, w_buffer.data(), MAX_PATH);
    if (ret == 0)
      return 0;

    // From Microsoft docs:

    //   If the buffer is too small to hold the module name, the string is
    //   truncated to nSize characters including the terminating null character,
    //   the function returns nSize, and the function sets the last error to
    //   ERROR_INSUFFICIENT_BUFFER.
    if (ret >= nSize) {
      constexpr int max_unc_path = 32767;
      if (!w_buffer.resize(max_unc_path)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return 0;
      }
      ret = GetModuleFileNameW(hModule, w_buffer.data(), max_unc_path + 1);
      // GetModuleFileNameW can be thunk'ed and return 0 for ERROR_OUTOFMEMORY
      if (ret == 0)
        return 0;
    }

    d::u_str u_name;
    if (!u_name.from_w(w_buffer.data(), ret)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return 0;
    }

    if (u_name.size() >= nSize) {
      c::memcpy(lpFilename, u_name.data(), nSize - 1);
      lpFilename[nSize - 1] = 0;
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      return nSize;
    } else {
      c::memcpy(lpFilename, u_name.data(), u_name.size());
      lpFilename[u_name.size()] = 0;
      return u_name.size();
    }
  }
} // namespace mingw_thunk
