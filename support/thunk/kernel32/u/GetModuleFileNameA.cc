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
    stl::wstring w_buffer(MAX_PATH, 0);

    DWORD ret = GetModuleFileNameW(hModule, w_buffer.data(), MAX_PATH);
    if (ret == 0)
      return 0;

    // From Microsoft docs:

    //   If the buffer is too small to hold the module name, the string is
    //   truncated to nSize characters including the terminating null character,
    //   the function returns nSize, and the function sets the last error to
    //   ERROR_INSUFFICIENT_BUFFER.
    if (ret >= nSize) {
      constexpr int max_unc_path = 32767 + 1;
      w_buffer.resize(max_unc_path);
      ret = GetModuleFileNameW(hModule, w_buffer.data(), max_unc_path);
      if (ret == 0)
        return 0;
    }

    stl::string a_name = internal::w2u(w_buffer.data(), ret);
    if (a_name.size() >= nSize) {
      libc::memcpy(lpFilename, a_name.data(), nSize - 1);
      lpFilename[nSize - 1] = 0;
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      return nSize;
    } else {
      libc::memcpy(lpFilename, a_name.data(), a_name.size());
      lpFilename[a_name.size()] = 0;
      return a_name.size();
    }
  }
} // namespace mingw_thunk
