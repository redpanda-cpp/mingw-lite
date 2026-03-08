// workaround mingw-w64 11.0.1
#define STRSAFE_NO_DEPRECATE

#include "../string.h"

#include <strsafe.h>
#include <windows.h>

// libmingwex provides non-inline version of strsafe functions, which will
// cause link error if we use (by default, inline version of) them. Here we
// have to provide our version.

namespace mingw_thunk::internal
{
  inline HRESULT WINAPI StringCchLengthW(_In_ LPCWSTR psz,
                                         _In_ size_t cchMax,
                                         _Out_ size_t *pcchLength) noexcept
  {
    *pcchLength = c::wcsnlen(psz, cchMax);
    return S_OK;
  }

  inline HRESULT WINAPI StringCchCopyW(_Out_ LPWSTR pszDest,
                                       _In_ size_t cchDest,
                                       _In_ LPCWSTR pszSrc)
  {
    if (!pszDest || cchDest == 0 || cchDest > STRSAFE_MAX_CCH || !pszSrc)
      return STRSAFE_E_INVALID_PARAMETER;

    wchar_t *dend = pszDest + cchDest - 1;
    while (pszDest < dend && *pszSrc)
      *pszDest++ = *pszSrc++;
    *pszDest = 0;

    if (*pszSrc)
      return STRSAFE_E_INSUFFICIENT_BUFFER;

    return S_OK;
  }
} // namespace mingw_thunk::internal
