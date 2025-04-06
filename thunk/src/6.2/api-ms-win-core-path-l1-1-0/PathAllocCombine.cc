#include <thunk/_common.h>

#include <pathcch.h>
#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Minimum supported client	Windows 8 [desktop apps | UWP apps]
  // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
  __DEFINE_THUNK(api_ms_win_core_path_l1_1_0,
                 16,
                 HRESULT,
                 WINAPI,
                 PathAllocCombine,
                 _In_opt_ PCWSTR pszPathIn,
                 _In_opt_ PCWSTR pszMore,
                 _In_ ULONG dwFlags,
                 _Outptr_ PWSTR *ppszPathOut)
  {
    if (const auto pPathAllocCombine = try_get_PathAllocCombine()) {
      return pPathAllocCombine(pszPathIn, pszMore, dwFlags, ppszPathOut);
    }

    // 参数检查
    if (ppszPathOut == nullptr)
      return E_INVALIDARG;

    *ppszPathOut = nullptr;

    // 不能同时为 null
    if (pszPathIn == nullptr && pszMore == nullptr)
      return E_INVALIDARG;

    size_t cchPathIn = pszPathIn ? wcslen(pszPathIn) : 0;

    if (cchPathIn >= PATHCCH_MAX_CCH)
      return __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);

    if (cchPathIn)
      ++cchPathIn;

    size_t cchMore = pszMore ? wcslen(pszMore) : 0;

    if (cchMore >= PATHCCH_MAX_CCH)
      return __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);

    if (cchMore)
      ++cchMore;

    auto cchBufferSize = cchPathIn + cchMore;

    if (cchBufferSize == 0) {
      //"\\\0"
      cchBufferSize = 2;
    } else if (cchBufferSize > MAX_PATH &&
               (dwFlags & PATHCCH_ALLOW_LONG_PATHS)) {
      //"\\\\?\\UNC\0"
      cchBufferSize += 6;
    }

    // 保证边界依然在 最大值内
    const unsigned cchMaxPath =
        (dwFlags & PATHCCH_ALLOW_LONG_PATHS) ? PATHCCH_MAX_CCH : MAX_PATH;
    if (cchBufferSize > cchMaxPath)
      cchBufferSize = cchMaxPath;

    auto pBuffer =
        (wchar_t *)LocalAlloc(LMEM_ZEROINIT, sizeof(wchar_t) * cchBufferSize);

    if (pBuffer == nullptr)
      return E_OUTOFMEMORY;

    auto hr =
        PathCchCombineEx(pBuffer, cchBufferSize, pszPathIn, pszMore, dwFlags);

    if (FAILED(hr)) {
      LocalFree(pBuffer);
      return hr;
    }

    *ppszPathOut = pBuffer;

    return hr;
  }
} // namespace mingw_thunk
