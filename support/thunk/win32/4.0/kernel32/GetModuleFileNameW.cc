#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errhandlingapi.h>
#include <libloaderapi.h>

#include <nocrt/__wchar/wmemcpy.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 DWORD,
                 WINAPI,
                 GetModuleFileNameW,
                 _In_opt_ HMODULE hModule,
                 _Out_ LPWSTR lpFilename,
                 _In_ DWORD nSize)
  {
    if (internal::is_nt())
      return get_GetModuleFileNameW()(hModule, lpFilename, nSize);

    char a_filename[MAX_PATH];
    DWORD a_size = GetModuleFileNameA(hModule, a_filename, MAX_PATH);
    if (a_size == 0)
      return 0;
    stl::wstring w_filename = internal::widen(a_filename);
    if (nSize >= w_filename.size() + 1) {
      wmemcpy(lpFilename, w_filename.c_str(), w_filename.size());
      lpFilename[w_filename.size()] = L'\0';
      return w_filename.size();
    } else {
      wmemcpy(lpFilename, w_filename.c_str(), nSize - 1);
      lpFilename[nSize - 1] = L'\0';
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      return nSize;
    }
  }
} // namespace mingw_thunk
