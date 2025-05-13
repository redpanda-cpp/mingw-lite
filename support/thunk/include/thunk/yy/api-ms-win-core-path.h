#pragma once

#include <windows.h>

// YY-Thunks 1.1.6
namespace mingw_thunk::internal
{
  static bool __fastcall IsHexDigitWorker(wchar_t ch)
  {
    return (ch >= L'0' && ch <= L'9') || (ch >= L'A' && ch <= L'F') ||
           (ch >= L'a' && ch <= L'f');
  }

  static bool __fastcall StringIsGUIDWorker(LPCWSTR szStr)
  {
    return szStr[0] == L'{' && IsHexDigitWorker(szStr[1]) &&
           IsHexDigitWorker(szStr[2]) && IsHexDigitWorker(szStr[3]) &&
           IsHexDigitWorker(szStr[4]) && IsHexDigitWorker(szStr[5]) &&
           IsHexDigitWorker(szStr[6]) && IsHexDigitWorker(szStr[7]) &&
           IsHexDigitWorker(szStr[8]) && szStr[9] == L'-' &&
           IsHexDigitWorker(szStr[10]) && IsHexDigitWorker(szStr[11]) &&
           IsHexDigitWorker(szStr[12]) && IsHexDigitWorker(szStr[13]) &&
           szStr[14] == L'-' && IsHexDigitWorker(szStr[15]) &&
           IsHexDigitWorker(szStr[16]) && IsHexDigitWorker(szStr[17]) &&
           IsHexDigitWorker(szStr[18]) && szStr[19] == L'-' &&
           IsHexDigitWorker(szStr[20]) && IsHexDigitWorker(szStr[21]) &&
           IsHexDigitWorker(szStr[22]) && IsHexDigitWorker(szStr[23]) &&
           szStr[24] == L'-' && IsHexDigitWorker(szStr[25]) &&
           IsHexDigitWorker(szStr[26]) && IsHexDigitWorker(szStr[27]) &&
           IsHexDigitWorker(szStr[28]) && IsHexDigitWorker(szStr[29]) &&
           IsHexDigitWorker(szStr[30]) && IsHexDigitWorker(szStr[31]) &&
           IsHexDigitWorker(szStr[32]) && IsHexDigitWorker(szStr[33]) &&
           IsHexDigitWorker(szStr[34]) && IsHexDigitWorker(szStr[35]) &&
           IsHexDigitWorker(szStr[36]) && szStr[37] == L'}';
  }

  static bool __fastcall PathIsVolumeGUIDWorker(LPCWSTR pszPath)
  {
    return pszPath[0] == L'\\' && pszPath[1] == L'\\' && pszPath[2] == L'?' &&
           pszPath[3] == L'\\' && (pszPath[4] == L'V' || pszPath[4] == L'v') &&
           (pszPath[5] == L'O' || pszPath[5] == L'o') &&
           (pszPath[6] == L'L' || pszPath[6] == L'l') &&
           (pszPath[7] == L'U' || pszPath[7] == L'u') &&
           (pszPath[8] == L'M' || pszPath[8] == L'm') &&
           (pszPath[9] == L'E' || pszPath[9] == L'e') &&
           internal::StringIsGUIDWorker(pszPath + 10);
  }

  /*
  以下后缀是有效的
  ""
  后缀内容最多255个字符
  不能包含 ' ' , '\' 以及 '.'

  */
  static bool __fastcall IsValidExtensionWorker(LPCWSTR pszExt)
  {
    if (pszExt == nullptr)
      return false;

    if (*pszExt == L'\0')
      return true;

    if (*pszExt == L'.')
      ++pszExt;

    // 检查长度 以及非法字符检测
    const auto pszExtMax = pszExt + 256;

    for (;; ++pszExt) {
      // 长度超过 256
      if (pszExt >= pszExtMax) {
        return false;
      }

      if (*pszExt == L'\0')
        break;

      if (*pszExt == L'.' || *pszExt == L' ' || *pszExt == L'\\') {
        return false;
      }
    }

    return true;
  }

  // 长命名？
  static bool __fastcall IsExtendedLengthDosDevicePath(LPCWSTR pszPath)
  {
    return pszPath[0] == L'\\' && pszPath[1] == L'\\' && pszPath[2] == L'?' &&
           pszPath[3] == L'\\';
  }

  static LPCWSTR __fastcall FindPreviousBackslashWorker(LPCWSTR szPathStart,
                                                        LPCWSTR szPathEnd)
  {
    for (; szPathStart < szPathEnd;) {
      --szPathEnd;

      if (*szPathEnd == L'\\')
        return szPathEnd;
    }

    return nullptr;
  }

  static bool __fastcall PathIsRelativeWorker(LPCWSTR szPath)
  {
    return szPath[0] != L'\\' && (((L'A' <= szPath[0] && szPath[0] <= L'Z') ||
                                   (L'a' <= szPath[0] && szPath[0] <= L'z')) &&
                                  szPath[1] == L':') == false;
  }
} // namespace mingw_thunk::internal
