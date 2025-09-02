// workaround mingw-w64 11.0.1
#define STRSAFE_NO_DEPRECATE

#include <strsafe.h>
#include <windows.h>

#include <nocrt/wchar.h>

// libmingwex provides non-inline version of strsafe functions, which will
// cause link error if we use (by default, inline version of) them. Here we
// have to provide our version.

namespace mingw_thunk::internal
{
  inline HRESULT WINAPI StringCchLengthW(_In_ LPCWSTR psz,
                                         _In_ size_t cchMax,
                                         _Out_ size_t *pcchLength) noexcept
  {
    *pcchLength = libc::wcsnlen(psz, cchMax);
    return S_OK;
  }

  inline HRESULT WINAPI StringCchCopyW(_Out_ LPWSTR pszDest,
                                       _In_ size_t cchDest,
                                       _In_ LPCWSTR pszSrc)
  {
    wchar_t *end = libc::wcspncpy(pszDest, pszSrc, cchDest - 1);
    *end = 0;
    return (end - pszDest < cchDest - 1) ? S_OK : STRSAFE_E_INSUFFICIENT_BUFFER;
  }
} // namespace mingw_thunk::internal
