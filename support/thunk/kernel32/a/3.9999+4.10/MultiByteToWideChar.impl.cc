#include "MultiByteToWideChar.h"

#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>
#include <thunk/unicode.h>

#include <windows.h>

#define INVALID_RETURN()                                                       \
  do {                                                                         \
    if (dwFlags == MB_ERR_INVALID_CHARS) {                                     \
      SetLastError(ERROR_NO_UNICODE_TRANSLATION);                              \
      return 0;                                                                \
    }                                                                          \
  } while (0)

#define READ() lpMultiByteStr[i_idx++]

#define WRITE(ch)                                                              \
  do {                                                                         \
    if (cchWideChar == 0) {                                                    \
      o_idx++;                                                                 \
      break;                                                                   \
    }                                                                          \
    if (o_idx >= cchWideChar) {                                                \
      SetLastError(ERROR_INSUFFICIENT_BUFFER);                                 \
      return 0;                                                                \
    }                                                                          \
    lpWideCharStr[o_idx++] = ch;                                               \
  } while (0)

namespace mingw_thunk
{
  namespace f
  {
    int __stdcall win95_MultiByteToWideChar(_In_ UINT CodePage,
                                            _In_ DWORD dwFlags,
                                            _In_ LPCSTR lpMultiByteStr,
                                            _In_ int cbMultiByte,
                                            _Out_opt_ LPWSTR lpWideCharStr,
                                            _In_ int cchWideChar)
    {
      if (CodePage != CP_UTF8)
        return __ms_MultiByteToWideChar(CodePage,
                                        dwFlags,
                                        lpMultiByteStr,
                                        cbMultiByte,
                                        lpWideCharStr,
                                        cchWideChar);

      if (!(dwFlags == 0 || dwFlags == MB_ERR_INVALID_CHARS)) {
        SetLastError(ERROR_INVALID_FLAGS);
        return 0;
      }

      if (cbMultiByte == -1)
        cbMultiByte = c::strlen(lpMultiByteStr) + 1;

      int i_idx = 0;
      int o_idx = 0;
      while (i_idx < cbMultiByte) {
        int n_bytes = i::u8_dec_len(lpMultiByteStr[i_idx]);
        switch (n_bytes) {
        case 0: {
          INVALID_RETURN();
          i_idx += 1;
          WRITE(g::u16_rep);
          break;
        }
        case 1: {
          uint8_t b0 = READ();
          WRITE(b0);
          break;
        }
        case 2:
          if (i_idx + 2 > cbMultiByte ||
              !i::u8_is_trail(lpMultiByteStr[i_idx + 1])) {
            INVALID_RETURN();
            i_idx += 1;
            WRITE(g::u16_rep);
            break;
          }
          {
            uint8_t b0 = READ();
            uint8_t b1 = READ();
            WRITE(i::u8_dec_2({b0, b1}));
            break;
          }
        case 3:
          if (i_idx + 2 > cbMultiByte ||
              !i::u8_is_trail(lpMultiByteStr[i_idx + 1])) {
            INVALID_RETURN();
            i_idx += 1;
            WRITE(g::u16_rep);
            break;
          }
          if (i_idx + 3 > cbMultiByte ||
              !i::u8_is_trail(lpMultiByteStr[i_idx + 2])) {
            INVALID_RETURN();
            i_idx += 2;
            WRITE(g::u16_rep);
            break;
          }
          {
            uint8_t b0 = READ();
            uint8_t b1 = READ();
            uint8_t b2 = READ();
            WRITE(i::u8_dec_3({b0, b1, b2}));
            break;
          }
        case 4:
          if (i_idx + 2 > cbMultiByte ||
              !i::u8_is_trail(lpMultiByteStr[i_idx + 1])) {
            INVALID_RETURN();
            i_idx += 1;
            WRITE(g::u16_rep);
            break;
          }
          if (i_idx + 3 > cbMultiByte ||
              !i::u8_is_trail(lpMultiByteStr[i_idx + 2])) {
            INVALID_RETURN();
            i_idx += 2;
            WRITE(g::u16_rep);
            break;
          }
          if (i_idx + 4 > cbMultiByte ||
              !i::u8_is_trail(lpMultiByteStr[i_idx + 3])) {
            INVALID_RETURN();
            i_idx += 3;
            WRITE(g::u16_rep);
            break;
          }
          {
            uint8_t b0 = READ();
            uint8_t b1 = READ();
            uint8_t b2 = READ();
            uint8_t b3 = READ();
            char32_t ch = i::u8_dec_4({b0, b1, b2, b3});
            switch (i::u16_enc_len(ch)) {
            case 1: {
              WRITE(ch);
              break;
            }
            case 2: {
              auto s = i::u16_enc_2(ch);
              WRITE(s.high);
              WRITE(s.low);
              break;
            }
            default:
              __builtin_unreachable();
            }
            break;
          }
        default:
          __builtin_unreachable();
        }
      }
      return o_idx;
    }
  } // namespace f
} // namespace mingw_thunk
