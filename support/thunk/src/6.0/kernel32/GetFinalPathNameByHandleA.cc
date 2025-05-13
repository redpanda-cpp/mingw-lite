#include <thunk/_common.h>
#include <thunk/try_get.h>
#include <thunk/yy/yy.h>

#include <ntstatus.h>
#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Modification: use `GetProcessHeap()` instead of reserved PEB field.
  // Windows Vista,  Windows Server 2008
  __DEFINE_THUNK(kernel32,
                 16,
                 DWORD,
                 WINAPI,
                 GetFinalPathNameByHandleA,
                 _In_ HANDLE hFile,
                 _Out_writes_(cchFilePath) LPSTR lpszFilePath,
                 _In_ DWORD cchFilePath,
                 _In_ DWORD dwFlags)
  {
    if (auto pGetFinalPathNameByHandleA = try_get_GetFinalPathNameByHandleA()) {
      return pGetFinalPathNameByHandleA(
          hFile, lpszFilePath, cchFilePath, dwFlags);
    }

    const auto ProcessHeap = GetProcessHeap();
    wchar_t *szFilePathUnicode = nullptr;
    for (DWORD cchszFilePathUnicode = 1040;;) {
      if (szFilePathUnicode) {
        auto pNewBuffer =
            (wchar_t *)HeapReAlloc(ProcessHeap,
                                   0,
                                   szFilePathUnicode,
                                   cchszFilePathUnicode * sizeof(wchar_t));
        if (!pNewBuffer) {
          HeapFree(ProcessHeap, 0, szFilePathUnicode);
          SetLastError(ERROR_NOT_ENOUGH_MEMORY);
          return 0;
        }

        szFilePathUnicode = pNewBuffer;
      } else {
        szFilePathUnicode = (wchar_t *)HeapAlloc(
            ProcessHeap, 0, cchszFilePathUnicode * sizeof(wchar_t));
        if (!szFilePathUnicode) {
          SetLastError(ERROR_NOT_ENOUGH_MEMORY);
          return 0;
        }
      }

      auto cchReturn = GetFinalPathNameByHandleW(
          hFile, szFilePathUnicode, cchszFilePathUnicode, dwFlags);

      if (cchReturn == 0) {
      __Error:

        auto lStatus = GetLastError();
        HeapFree(ProcessHeap, 0, szFilePathUnicode);
        SetLastError(lStatus);

        return 0;
      } else if (cchReturn > cchszFilePathUnicode) {
        // 缓冲区不足
        cchszFilePathUnicode = cchReturn;
        continue;
      } else {
        // 操作成功！
        const UINT CodePage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

        auto cchReturnANSI = WideCharToMultiByte(CodePage,
                                                 WC_NO_BEST_FIT_CHARS,
                                                 szFilePathUnicode,
                                                 cchReturn,
                                                 nullptr,
                                                 0,
                                                 nullptr,
                                                 nullptr);

        if (cchReturnANSI <= 0) {
          goto __Error;
        } else if (static_cast<DWORD>(cchReturnANSI) >= cchFilePath) {
          // 长度不足
          ++cchReturnANSI;
        } else {
          WideCharToMultiByte(CodePage,
                              WC_NO_BEST_FIT_CHARS,
                              szFilePathUnicode,
                              cchReturn,
                              lpszFilePath,
                              cchFilePath,
                              nullptr,
                              nullptr);
          lpszFilePath[cchReturnANSI] = '\0';
        }

        HeapFree(ProcessHeap, 0, szFilePathUnicode);
        return cchReturnANSI;
      }
    }
  }
} // namespace mingw_thunk
