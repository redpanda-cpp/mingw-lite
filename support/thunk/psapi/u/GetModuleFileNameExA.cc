#include <thunk/_common.h>
#include <thunk/string.h>

#define PSAPI_VERSION 1

#include <psapi.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(psapi,
                 16,
                 DWORD,
                 WINAPI,
                 GetModuleFileNameExA,
                 _In_ HANDLE hProcess,
                 _In_opt_ HMODULE hModule,
                 _Out_ LPSTR lpFilename,
                 _In_ DWORD nSize)
  {
    // dry run for buffer size
    if (nSize && !lpFilename) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return 0;
    }

    d::w_str w_buffer{d::max_path_tag{}};

    DWORD ret =
        GetModuleFileNameExW(hProcess, hModule, w_buffer.data(), MAX_PATH);
    if (ret == 0)
      return 0;

    // From Microsoft docs:

    //   If the size of the file name is larger than the value of the nSize
    //   parameter, the function succeeds but the file name is truncated and
    //   null-terminated.

    //   If the function succeeds, the return value specifies the length of the
    //   string copied to the buffer.
    if (ret >= nSize - 1) {
      constexpr int max_unc_path = 32767;
      if (!w_buffer.resize(max_unc_path)) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
      }
      ret = GetModuleFileNameExW(
          hProcess, hModule, w_buffer.data(), max_unc_path);
      // GetModuleFileNameExW can be thunk'ed and return 0 for
      // ERROR_OUTOFMEMORY
      if (ret == 0)
        return 0;
    }

    d::u_str u_name;
    if (!u_name.from_w(w_buffer.c_str(), ret))
      return 0;
    if (u_name.size() >= nSize)
      u_name.resize(nSize - 1);

    c::memcpy(lpFilename, u_name.data(), u_name.size());
    lpFilename[u_name.size()] = 0;
    return u_name.size();
  }
} // namespace mingw_thunk
