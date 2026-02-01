#include "WideCharToMultiByte.h"

#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/unicode.h>

#include <nocrt/wchar.h>

#include <windows.h>

#define INVALID_RETURN()                                                       \
  do {                                                                         \
    if (dwFlags == WC_ERR_INVALID_CHARS) {                                     \
      SetLastError(ERROR_NO_UNICODE_TRANSLATION);                              \
      return 0;                                                                \
    }                                                                          \
  } while (0)

#define READ() lpWideCharStr[i_idx++]

#define WRITE(b)                                                               \
  do {                                                                         \
    if (cbMultiByte == 0) {                                                    \
      o_idx++;                                                                 \
      break;                                                                   \
    }                                                                          \
    if (o_idx >= cbMultiByte) {                                                \
      SetLastError(ERROR_INSUFFICIENT_BUFFER);                                 \
      return 0;                                                                \
    }                                                                          \
    lpMultiByteStr[o_idx++] = b;                                               \
  } while (0)

namespace mingw_thunk
{
  namespace impl
  {
    int win95_WideCharToMultiByte(_In_ UINT CodePage,
                                  _In_ DWORD dwFlags,
                                  _In_ LPCWCH lpWideCharStr,
                                  _In_ int cchWideChar,
                                  _Out_opt_ LPSTR lpMultiByteStr,
                                  _In_ int cbMultiByte,
                                  _In_opt_ LPCCH lpDefaultChar,
                                  _In_opt_ LPBOOL lpUsedDefaultChar)
    {
      if (CodePage != CP_UTF8)
        return __ms_WideCharToMultiByte(CodePage,
                                        dwFlags,
                                        lpWideCharStr,
                                        cchWideChar,
                                        lpMultiByteStr,
                                        cbMultiByte,
                                        lpDefaultChar,
                                        lpUsedDefaultChar);

      if (!(dwFlags == 0 || dwFlags == WC_ERR_INVALID_CHARS)) {
        SetLastError(ERROR_INVALID_FLAGS);
        return 0;
      }

      if (cchWideChar == -1)
        cchWideChar = libc::wcslen(lpWideCharStr) + 1;

      int i_idx = 0;
      int o_idx = 0;
      while (i_idx < cchWideChar) {
        if (internal::u16_is_low(lpWideCharStr[i_idx])) {
          INVALID_RETURN();
          i_idx += 1;
          WRITE(internal::u8_rep.b0);
          WRITE(internal::u8_rep.b1);
          WRITE(internal::u8_rep.b2);
          continue;
        }
        if (internal::u16_is_high(lpWideCharStr[i_idx])) {
          if (i_idx + 2 > cchWideChar ||
              !internal::u16_is_low(lpWideCharStr[i_idx + 1])) {
            INVALID_RETURN();
            i_idx += 1;
            WRITE(internal::u8_rep.b0);
            WRITE(internal::u8_rep.b1);
            WRITE(internal::u8_rep.b2);
            continue;
          }
          char16_t high = READ();
          char16_t low = READ();
          char32_t ch = internal::u16_dec_2({high, low});
          auto s = internal::u8_enc_4(ch);
          WRITE(s.b0);
          WRITE(s.b1);
          WRITE(s.b2);
          WRITE(s.b3);
          continue;
        }
        char16_t ch = READ();
        switch (internal::u8_enc_len(ch)) {
        case 1: {
          WRITE(ch);
          break;
        }
        case 2: {
          auto s = internal::u8_enc_2(ch);
          WRITE(s.b0);
          WRITE(s.b1);
          break;
        }
        case 3: {
          auto s = internal::u8_enc_3(ch);
          WRITE(s.b0);
          WRITE(s.b1);
          WRITE(s.b2);
          break;
        }
        default:
          __builtin_unreachable();
        }
      }
      return o_idx;
    }
  } // namespace impl
} // namespace mingw_thunk
