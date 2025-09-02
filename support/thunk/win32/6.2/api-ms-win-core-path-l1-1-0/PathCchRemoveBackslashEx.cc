#include <thunk/_common.h>

#include <pathcch.h>
#include <windows.h>

#include <nocrt/wchar.h>

namespace mingw_thunk
{
  // YY-Thunk 1.1.6
  // Minimum supported client	Windows 8 [desktop apps | UWP apps]
  // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
  __DEFINE_THUNK(api_ms_win_core_path_l1_1_0,
                 16,
                 HRESULT,
                 WINAPI,
                 PathCchRemoveBackslashEx,
                 _Inout_updates_(_Inexpressible_(cchPath)) PWSTR pszPath,
                 _In_ size_t cchPath,
                 _Outptr_opt_result_buffer_(*pcchRemaining) PWSTR *ppszEnd,
                 _Out_opt_ size_t *pcchRemaining)
  {
    if (ppszEnd)
      *ppszEnd = nullptr;

    if (pcchRemaining)
      *pcchRemaining = 0;

    auto nLength = libc::wcslen(pszPath);

    HRESULT hr = S_FALSE;

    do {
      if (nLength >= cchPath) {
        hr = E_INVALIDARG;
        break;
      }

      if (nLength && pszPath[nLength - 1] == L'\\') {
        --nLength;

        if (!PathCchIsRoot(pszPath)) {
          pszPath[nLength] = L'\0';
          hr = S_OK;
        }
      }

      if (ppszEnd)
        *ppszEnd = pszPath + nLength;

      if (pcchRemaining)
        *pcchRemaining = cchPath - nLength;

    } while (false);

    return hr;
  }
} // namespace mingw_thunk
