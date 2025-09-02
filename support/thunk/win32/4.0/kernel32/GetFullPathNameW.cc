#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

#include <nostl/string.h>

#include <nocrt/string.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 16,
                 DWORD,
                 WINAPI,
                 GetFullPathNameW,
                 _In_ LPCWSTR lpFileName,
                 _In_ DWORD nBufferLength,
                 _Out_ LPWSTR lpBuffer,
                 _Out_opt_ LPWSTR *lpFilePart)
  {
    if (internal::is_nt())
      return get_GetFullPathNameW()(
          lpFileName, nBufferLength, lpBuffer, lpFilePart);

    auto aname = internal::narrow(lpFileName);
    size_t abuflen = nBufferLength > MAX_PATH ? nBufferLength : MAX_PATH;
    stl::string abuf(abuflen, 0);
    char *afilepart = nullptr;
    DWORD alen =
        GetFullPathNameA(aname.c_str(), abuflen, abuf.data(), &afilepart);
    if (alen == 0) {
      return FALSE;
    }
    if (alen > abuflen) {
      abuf.resize(alen);
      alen = GetFullPathNameA(aname.c_str(), abuflen, abuf.data(), &afilepart);
    }
    size_t aprefixlen = afilepart - abuf.data();
    abuf.resize(alen); // shrink

    auto wres = internal::widen(abuf.data(), alen);

    size_t len = wres.length() + 1;
    if (len > nBufferLength)
      return len;

    if (lpFilePart) {
      auto wprefix = internal::widen(abuf.data(), aprefixlen);
      *lpFilePart = lpBuffer + wprefix.length();
    }

    libc::memcpy(lpBuffer, wres.data(), len * sizeof(wchar_t));
    return wres.length();
  }
} // namespace mingw_thunk
