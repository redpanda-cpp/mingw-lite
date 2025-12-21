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
    stl::wstring w_buffer(MAX_PATH, 0);

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
      constexpr int max_unc_path = 32767 + 1;
      w_buffer.resize(max_unc_path);
      ret = GetModuleFileNameExW(
          hProcess, hModule, w_buffer.data(), max_unc_path);
      if (ret == 0)
        return 0;
    }

    stl::string a_name = internal::w2u(w_buffer.data(), ret);
    if (a_name.size() >= nSize)
      a_name.resize(nSize - 1);

    libc::memcpy(lpFilename, a_name.data(), a_name.size());
    lpFilename[a_name.size()] = 0;
    return a_name.size();
  }
} // namespace mingw_thunk
