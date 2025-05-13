#include <thunk/_common.h>

#include <pathcch.h>
#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Minimum supported client	Windows 8 [desktop apps | UWP apps]
  // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
  __DEFINE_THUNK(api_ms_win_core_path_l1_1_0,
                 12,
                 HRESULT,
                 WINAPI,
                 PathCchFindExtension,
                 _In_reads_(_Inexpressible_(cchPath)) PCWSTR pszPath,
                 _In_ size_t cchPath,
                 _Outptr_ PCWSTR *ppszExt)
  {
    if (ppszExt == nullptr)
      return E_INVALIDARG;

    *ppszExt = nullptr;

    if (pszPath == nullptr || cchPath == 0 || cchPath > PATHCCH_MAX_CCH) {
      return E_INVALIDARG;
    }

    const auto szPathBufferEnd = pszPath + cchPath;
    PCWSTR pszExt = nullptr;

    for (; *pszPath; ++pszPath) {
      if (pszPath >= szPathBufferEnd)
        return E_INVALIDARG;

      switch (*pszPath) {
      case L'\\':
      case L' ':
        pszExt = nullptr;
        break;
      case L'.':
        pszExt = pszPath;
        break;
      default:
        break;
      }
    }

    *ppszExt = pszExt ? pszExt : pszPath;

    return S_OK;
  }
} // namespace mingw_thunk
