#include <thunk/_common.h>
#include <thunk/yy/api-ms-win-core-path.h>

#include <stdlib.h>

#include <pathcch.h>
#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Minimum supported client	Windows 8 [desktop apps | UWP apps]
  // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
  __DEFINE_THUNK(api_ms_win_core_path_l1_1_0,
                 20,
                 HRESULT,
                 WINAPI,
                 PathCchCombineEx,
                 _Out_writes_(cchPathOut) PWSTR pszPathOut,
                 _In_ size_t cchPathOut,
                 _In_opt_ PCWSTR pszPathIn,
                 _In_opt_ PCWSTR pszMore,
                 _In_ ULONG dwFlags)
  {
    if (pszPathOut == nullptr || cchPathOut == 0 ||
        cchPathOut > PATHCCH_MAX_CCH) {
      return E_INVALIDARG;
    }

    wchar_t PathOutStaticBuffer[MAX_PATH];
    PWSTR pszPathOutTmp = nullptr;
    size_t cchPathOutTmp = 0;

    HRESULT hr = S_OK;

    do {
      if (pszPathIn == nullptr && pszMore == nullptr) {
        hr = E_INVALIDARG;
        break;
      }

      size_t cchPathIn = pszPathIn ? wcslen(pszPathIn) : 0;
      if (cchPathIn >= PATHCCH_MAX_CCH) {
        hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
        break;
      }

      size_t cchMore = pszMore ? wcslen(pszMore) : 0;
      if (cchMore >= PATHCCH_MAX_CCH) {
        hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
        break;
      }

      // 极端情况可能需要额外添加 '\\' 连接，简化处理，直接 +
      // 1。改逻辑与微软不同。 字符串末尾需要 '\0' 结束
      const auto cchPathInNeed = cchPathIn + cchMore + 1 + 1;

      if (cchPathInNeed < _countof(PathOutStaticBuffer)) {
        // 静态缓冲区可以容纳，我们直接使用静态缓冲区
        pszPathOutTmp = PathOutStaticBuffer;
      } else {
        // 好吧，我们真的需要开辟临时缓冲区
        pszPathOutTmp =
            (PWSTR)LocalAlloc(LMEM_ZEROINIT, cchPathInNeed * sizeof(wchar_t));
        if (!pszPathOutTmp) {
          hr = E_OUTOFMEMORY;
          break;
        }
      }

      if (cchPathIn == 0) {
        memcpy(pszPathOutTmp, pszMore, cchMore * sizeof(wchar_t));
        cchPathOutTmp = cchMore;
      } else if (cchMore == 0) {
        memcpy(pszPathOutTmp, pszPathIn, cchPathIn * sizeof(wchar_t));
        cchPathOutTmp = cchPathIn;
      } else {
        if (internal::PathIsRelativeWorker(pszMore)) {
          memcpy(pszPathOutTmp, pszPathIn, cchPathIn * sizeof(wchar_t));
          cchPathOutTmp = cchPathIn;
        } else {
          if (pszMore[0] == L'\\' && pszMore[1] != L'\\') {
            // 路径是 '\\' 开头，那么 它与 pszPathIn 的根目录进行合并
            ++pszMore;
            --cchMore;

            PCWSTR pszRootEnd;
            hr = PathCchSkipRoot(pszPathIn, &pszRootEnd);
            if (FAILED(hr)) {
              break;
            }

            cchPathOutTmp = pszRootEnd - pszPathIn;
            memcpy(pszPathOutTmp, pszPathIn, cchPathOutTmp * sizeof(wchar_t));
          } else {
            // 这是一个绝对的路径，我们只需要把 pszMore 复制到 pszPathOutTmp
            memcpy(pszPathOutTmp, pszMore, cchMore * sizeof(wchar_t));
            cchPathOutTmp = cchMore;
            break;
          }
        }

        /*
        * 不想调用它……只是追加一个 '\' 而已
        PWSTR pszEnd;
        hr = PathCchAddBackslashEx(pszPathOutTmp, cchPathOutTmp, &pszEnd,
        &cchPathOutTmp); if (FAILED(hr))
        {
                break;
        }
        */
        if (cchPathOutTmp && pszPathOutTmp[cchPathOutTmp - 1] != L'\\') {
          pszPathOutTmp[cchPathOutTmp] = L'\\';
          ++cchPathOutTmp;
        }

        memcpy(
            pszPathOutTmp + cchPathOutTmp, pszMore, cchMore * sizeof(wchar_t));
        cchPathOutTmp += cchMore;
      }

    } while (false);

    if (FAILED(hr)) {
      *pszPathOut = L'\0';
    } else {
      // 保证字符串 '\0' 截断
      pszPathOutTmp[cchPathOutTmp] = L'\0';
      hr =
          PathCchCanonicalizeEx(pszPathOut, cchPathOut, pszPathOutTmp, dwFlags);
    }

    if (pszPathOutTmp && pszPathOutTmp != PathOutStaticBuffer)
      LocalFree(pszPathOutTmp);

    return hr;
  }
} // namespace mingw_thunk
