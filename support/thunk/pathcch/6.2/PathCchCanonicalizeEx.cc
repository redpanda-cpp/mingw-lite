#include <thunk/_common.h>
#include <thunk/yy/api-ms-win-core-path.h>
#include <thunk/yy/strsafe.h>
#include <thunk/yy/yy.h>

#include <stdlib.h>

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
                 APIENTRY,
                 PathCchCanonicalizeEx,
                 _Out_writes_(cchPathOut) PWSTR pszPathOut,
                 _In_ size_t cchPathOut,
                 _In_ PCWSTR pszPathIn,
                 _In_ ULONG dwFlags)
  {
    if (const auto pPathCchCanonicalizeEx = try_get_PathCchCanonicalizeEx()) {
      return pPathCchCanonicalizeEx(pszPathOut, cchPathOut, pszPathIn, dwFlags);
    }

    if (pszPathOut && cchPathOut)
      *pszPathOut = L'\0';

    if (cchPathOut == 0 || cchPathOut > PATHCCH_MAX_CCH) {
      return E_INVALIDARG;
    }

    // MAX_PATH 长度的路径重新禁止长命名
    if (cchPathOut <= MAX_PATH)
      dwFlags &= ~PATHCCH_ALLOW_LONG_PATHS;
    else if ((dwFlags & PATHCCH_ALLOW_LONG_PATHS) == 0)
      cchPathOut = MAX_PATH;

    DWORD ulReservedSize = 0;

    auto pTempOut = pszPathOut;
    const auto pszPathOutEnd = pszPathOut + cchPathOut;

    HRESULT hr = S_OK;

    do {
      PCWSTR pszServer;
      const auto bUNC = PathIsUNCEx(pszPathIn, &pszServer);
      if (bUNC) {
        if (dwFlags & PATHCCH_ALLOW_LONG_PATHS) {
          ulReservedSize = 6;

          if (pTempOut + 8 >= pszPathOutEnd) {
            hr = STRSAFE_E_INSUFFICIENT_BUFFER;
          } else {
            *pTempOut++ = L'\\';
            *pTempOut++ = L'\\';
            *pTempOut++ = L'?';
            *pTempOut++ = L'\\';
            *pTempOut++ = L'U';
            *pTempOut++ = L'N';
            *pTempOut++ = L'C';
            *pTempOut++ = L'\\';
            *pTempOut = L'\0';
          }
        } else {
          if (pTempOut + 2 >= pszPathOutEnd) {
            hr = STRSAFE_E_INSUFFICIENT_BUFFER;
          } else {
            *pTempOut++ = L'\\';
            *pTempOut++ = L'\\';
            *pTempOut = L'\0';
          }
        }
      } else {
        pszServer = pszPathIn;

        auto bSupportDosDevicePath = false;

        if (internal::IsExtendedLengthDosDevicePath(pszPathIn)) {
          if ((L'A' <= pszPathIn[4] && pszPathIn[4] <= L'Z') ||
              (L'a' <= pszPathIn[4] && pszPathIn[4] <= L'z') &&
                  pszPathIn[5] == L':') {
            pszServer += 4;
            bSupportDosDevicePath = true;
          }
        } else if ((L'A' <= pszPathIn[0] && pszPathIn[0] <= L'Z') ||
                   (L'a' <= pszPathIn[0] && pszPathIn[0] <= L'z') &&
                       pszPathIn[1] == L':') {
          bSupportDosDevicePath = true;
        }

        if (bSupportDosDevicePath && (dwFlags & PATHCCH_ALLOW_LONG_PATHS)) {
          ulReservedSize = 4;

          if (pTempOut + 4 >= pszPathOutEnd) {
            hr = STRSAFE_E_INSUFFICIENT_BUFFER;
          } else {
            *pTempOut++ = L'\\';
            *pTempOut++ = L'\\';
            *pTempOut++ = L'?';
            *pTempOut++ = L'\\';
            *pTempOut = L'\0';
          }
        }
      }

      // LABEL_49
      // 剩余部分是否 <=
      // MAX_PATH，如果满足条件，那么应该重新开始。并且将路径限制在 MAX_PATH
      if ((dwFlags & PATHCCH_ALLOW_LONG_PATHS) && ulReservedSize &&
          cchPathOut <= ulReservedSize + MAX_PATH) {
        if (cchPathOut > MAX_PATH)
          cchPathOut = MAX_PATH;

        ulReservedSize = 0;
        pszServer = pszPathIn;

        pTempOut = pszPathOut;

        // 缓冲区必然 >= 1 ,所以我们可以直接设置。
        *pTempOut = L'\0';
        // 清除错误，方便我们重新开始。
        hr = S_OK;
      }

      if (FAILED(hr))
        break;

      for (; *pszServer;) {
        /* 获取下一个文件名，故意不用微软的方案，而且效率低……
         * 遇到 '\\' 则说明文件名结束
         * 遇到 '\0' 则说明路径也结束了
         */
        unsigned cchFileName = 0;
        for (; pszServer[cchFileName] && pszServer[cchFileName] != L'\\';
             ++cchFileName) {
          /* 安全检查
           * 1. 普通路径中，文件名最多 256个字符
           * 2. 路径最大处理长度为 PATHCCH_MAX_CCH
           */

          if (((dwFlags & PATHCCH_ALLOW_LONG_PATHS) == 0 &&
               cchFileName > MAXIMUM_FILENAME_LENGTH) ||
              cchFileName >= PATHCCH_MAX_CCH) {
            hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
            goto OnFailedLabel;
          }
        }

        if (cchFileName == 1 && pszServer[0] == L'.') {
          // 表示当前目录

          ++pszServer;

          if (*pszServer) {
            // 说明这里是 '\\'，继续跳过
            ++pszServer;
          } else {
            // 这里是 L'\0'

            if (pTempOut > pszPathOut && PathCchIsRoot(pszPathOut) == FALSE) {
              // 不是跟目录，而且结束，那么需要删除最后的 '\\'
              --pTempOut;
              *pTempOut = L'\0';
            }
          }

          continue;
        } else if (cchFileName == 2 && pszServer[0] == L'.' &&
                   pszServer[1] == L'.') {
          // 表示上级目录

          pszServer += 2;

          if (pTempOut <= pszPathOut || PathCchIsRoot(pszPathOut)) {
            // 文件是根目录，或者开始位置时，遇到 ".." 则直接忽略即可。

            if (*pszServer == L'\\')
              ++pszServer;
          } else {
            auto szPrevious = (wchar_t *)internal::FindPreviousBackslashWorker(
                pszPathOut, pTempOut - 1);

            pTempOut = szPrevious ? szPrevious : pszPathOut;

            *pTempOut = L'\0';
          }

          continue;
        } else {
          if (cchFileName == 0 && pszServer[0] == L'\\') {
            cchFileName = 1;
          }

          if (pTempOut + cchFileName >= pszPathOutEnd) {
            // 空间不足，特殊判断一下，某些特定的形式是可以处理的。
            if (cchFileName == 1 && pszServer[0] == L'\\') {
              if (pszServer[1] == L'\0' ||
                  (pszServer[1] == L'.' && pszServer[2] == L'\0')) {
                // 可以确认，路径已经结束，所以无需再添加 '\\'
                break;
              } else if (pTempOut + 1 == pszPathOutEnd &&
                         pszServer[1] == L'.' && pszServer[2] == L'.') {
                // 下一个项目是返回上一个项目，所以可以伪造一下，直接设置为
                // L'\0' 然后往下移动
                ++pszServer;
                *pTempOut++ = L'\0';
                continue;
              }
            }

            hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
            goto OnFailedLabel;
          }

          // 复制文件名
          memcpy(pTempOut, pszServer, cchFileName * sizeof(pszServer[0]));
          pszServer += cchFileName;
          pTempOut += cchFileName;
          *pTempOut = L'\0';
        }
      }

      /* 删除 路径末尾的 .
       * "*." 则保留
       */
      if (pTempOut > pszPathOut) {
        auto Str = pTempOut;
        --Str;

        if (*Str == L'.') {
          for (;;) {
            if (Str == pszPathOut) {
              *Str = L'\0';
              pTempOut = Str;
              break;
            }

            --Str;

            if (*Str != L'*') {
              pTempOut = Str + 1;
              *pTempOut = L'\0';

              if (*Str == L'.')
                continue;
            }

            // goto LABEL_29;
            break;
          }
        }
      }

      // LABEL_29:

      constexpr int nCountOfData = _countof(L"::$DATA") - 1;

      if (pszPathOut + nCountOfData <= pTempOut && pTempOut[-7] == L':' &&
          pTempOut[-6] == L':' && pTempOut[-5] == L'$' &&
          (pTempOut[-4] == L'D' || pTempOut[-3] == L'd') &&
          (pTempOut[-3] == L'A' || pTempOut[-3] == L'a') &&
          (pTempOut[-2] == L'T' || pTempOut[-1] == L't') &&
          (pTempOut[-1] == L'A' || pTempOut[-1] == L'a')) {
        pTempOut -= nCountOfData;
        *pTempOut = L'\0';
      }

      // 处于长命名模式中，如果规范化后的路径有效部分小于
      // MAX_PATH，那么删除长命名前缀
      if (ulReservedSize) {
        if (size_t(pTempOut - pszPathOut) <= ulReservedSize + MAX_PATH) {
          if (bUNC) {
            // "\\\\?\\UNC\\"
            memmove(pszPathOut + 2,
                    pszPathOut + 8,
                    (pTempOut - (pszPathOut + 8) + 1) * sizeof(pTempOut[0]));
          } else {
            memmove(pszPathOut,
                    pszPathOut + 4,
                    (pTempOut - (pszPathOut + 4) + 1) * sizeof(pTempOut[0]));
          }
        }
      }

      if (cchPathOut > 1 && *pszPathOut == L'\0') {
        // 如果路径为 空，则转换为 "\\"
        pszPathOut[0] = L'\\';
        pszPathOut[1] = L'\0';
      } else if (cchPathOut > 3 && pszPathOut[1] == L':' &&
                 pszPathOut[2] == L'\0') {
        // 如果路径为 "X:"，则转换为 "X:\\"
        pszPathOut[2] = L'\\';
        pszPathOut[3] = L'\0';
      }

      return S_OK;

    } while (false);

    // LABEL_96 失败
  OnFailedLabel:

    *pszPathOut = L'\0';

    if (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
      if (dwFlags & PATHCCH_ALLOW_LONG_PATHS) {
        if (cchPathOut == PATHCCH_MAX_CCH)
          hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
      } else {
        if (cchPathOut == MAX_PATH)
          hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
      }
    }

    return hr;
  }
} // namespace mingw_thunk
