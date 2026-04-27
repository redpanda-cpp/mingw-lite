#include <iconv.h>

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "encoding.h"
#include "unicode.h"

namespace libiconv
{
  struct decode_result
  {
    uint32_t codepoint;
    int advance;
    bool ok;
    bool incomplete;
  };

  static int
  source_char_len(code_page_t page, const uint8_t *src, size_t remaining)
  {
    if (remaining == 0)
      return 1;

    uint8_t b = src[0];

    if (page_is_u18030(page)) {
      if (remaining >= 2)
        return u18030_dec_len(b, src[1]);
      else
        return u18030_dec_len(b);
    }

    if (IsDBCSLeadByteEx(page, b))
      return 2;

    return 1;
  }

  static uint16_t read_u16_unit(const uint8_t p[2], bool be)
  {
    uint16_t b0 = p[0];
    uint16_t b1 = p[1];

    if (be)
      return (b0 << 8) | b1;
    else
      return (b1 << 8) | b0;
  }

  static uint32_t read_u32_unit(const uint8_t p[4], bool be)
  {
    uint32_t b0 = p[0];
    uint32_t b1 = p[1];
    uint32_t b2 = p[2];
    uint32_t b3 = p[3];

    if (be)
      return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
    else
      return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
  }

  static decode_result
  decode_one(code_page_t page, const uint8_t cs[], size_t cs_len)
  {
    if (page_is_u8(page)) {
      int len = u8_dec_len(cs[0]);

      if (len < 0)
        return {0, 1, false, false};

      if ((size_t)len > cs_len)
        return {0, 0, false, true};

      for (int i = 1; i < len; i++) {
        if ((cs[i] & 0b1100'0000) != 0b1000'0000)
          return {0, 1, false, false};
      }

      uint32_t cp;
      switch (len) {
      case 1:
        cp = cs[0];
        break;
      case 2:
        cp = u8_dec(cs[0], cs[1]);
        break;
      case 3:
        cp = u8_dec(cs[0], cs[1], cs[2]);
        break;
      case 4:
        cp = u8_dec(cs[0], cs[1], cs[2], cs[3]);
        break;
      default:
        return {0, 1, false, false};
      }

      if (u8_enc_len(cp) != len)
        return {0, len, false, false};

      if (cp_out_of_range(cp) || cp_not_character(cp) || cp_is_surrogate(cp))
        return {0, len, false, false};

      return {cp, len, true, false};
    } else if (page_is_u16(page)) {
      if (cs_len < 2)
        return {0, 0, false, true};

      bool be = page_is_be(page);
      uint16_t cu = read_u16_unit(cs, be);

      if (u16_is_surrogate_lo(cu))
        return {0, 2, false, false};

      if (u16_is_surrogate_hi(cu)) {
        if (cs_len < 4)
          return {0, 0, false, true};

        uint16_t low = read_u16_unit(cs + 2, be);
        if (!u16_is_surrogate_lo(low))
          return {0, 2, false, false};

        uint32_t cp = u16_dec(cu, low);
        if (cp_not_character(cp))
          return {0, 4, false, false};

        return {cp, 4, true, false};
      } else {
        if (cp_not_character(cu))
          return {0, 2, false, false};

        return {cu, 2, true, false};
      }
    } else if (page_is_u32(page)) {
      if (cs_len < 4)
        return {0, 0, false, true};

      bool be = page_is_be(page);
      uint32_t cp = read_u32_unit(cs, be);

      if (cp_out_of_range(cp) || cp_not_character(cp) || cp_is_surrogate(cp))
        return {0, 4, false, false};

      return {cp, 4, true, false};
    } else {
      int len = source_char_len(page, cs, cs_len);
      if (len > (int)cs_len)
        return {0, 0, false, true};

      wchar_t wc[2];
      int wlen = MultiByteToWideChar(
          page, MB_ERR_INVALID_CHARS, (const char *)cs, len, wc, 2);
      if (wlen == 0)
        return {0, 1, false, false};

      if (wlen == 2 && u16_is_surrogate_hi(wc[0]) &&
          u16_is_surrogate_lo(wc[1])) {
        uint32_t cp = u16_dec(wc[0], wc[1]);
        if (cp_not_character(cp))
          return {0, len, false, false};

        return {cp, len, true, false};
      }

      if (cp_not_character(wc[0]) || cp_is_surrogate(wc[0]))
        return {0, len, false, false};

      return {wc[0], len, true, false};
    }
  }

  static void write_u16_unit(uint16_t cu, uint8_t p[2], bool be)
  {
    if (be) {
      p[0] = cu >> 8;
      p[1] = cu;
    } else {
      p[0] = cu;
      p[1] = cu >> 8;
    }
  }

  static void write_u32_unit(uint32_t cu, uint8_t p[4], bool be)
  {
    if (be) {
      p[0] = cu >> 24;
      p[1] = cu >> 16;
      p[2] = cu >> 8;
      p[3] = cu;
    } else {
      p[0] = cu;
      p[1] = cu >> 8;
      p[2] = cu >> 16;
      p[3] = cu >> 24;
    }
  }

  static int encode_one(iconv_descriptor *cd,
                        uint32_t cp,
                        uint8_t *outbuf[],
                        size_t *outbytesleft,
                        size_t *non_reversible)
  {
    code_page_t page = cd->to_page;

    if (page_is_u8(page)) {
      int len = u8_enc_len(cp);

      if (*outbytesleft < (size_t)len) {
        errno = E2BIG;
        return -1;
      }

      switch (len) {
      case 1:
        (*outbuf)[0] = (uint8_t)cp;
        break;
      case 2: {
        u8_2 r = u8_enc_2(cp);
        (*outbuf)[0] = r.b0;
        (*outbuf)[1] = r.b1;
        break;
      }
      case 3: {
        u8_3 r = u8_enc_3(cp);
        (*outbuf)[0] = r.b0;
        (*outbuf)[1] = r.b1;
        (*outbuf)[2] = r.b2;
        break;
      }
      case 4: {
        u8_4 r = u8_enc_4(cp);
        (*outbuf)[0] = r.b0;
        (*outbuf)[1] = r.b1;
        (*outbuf)[2] = r.b2;
        (*outbuf)[3] = r.b3;
        break;
      }
      }

      *outbuf += len;
      *outbytesleft -= len;
      return 0;
    } else if (page_is_u16(page)) {
      bool be = page_is_be(page);

      if (u16_enc_len(cp) == 2) {
        if (*outbytesleft < 4) {
          errno = E2BIG;
          return -1;
        }

        u16_2 cu = u16_enc_2(cp);
        write_u16_unit(cu.cu0, *outbuf, be);
        write_u16_unit(cu.cu1, *outbuf + 2, be);
        *outbuf += 4;
        *outbytesleft -= 4;
      } else {
        if (*outbytesleft < 2) {
          errno = E2BIG;
          return -1;
        }

        write_u16_unit(cp, *outbuf, be);
        *outbuf += 2;
        *outbytesleft -= 2;
      }

      return 0;
    } else if (page_is_u32(page)) {
      if (*outbytesleft < 4) {
        errno = E2BIG;
        return -1;
      }

      bool be = page_is_be(page);
      write_u32_unit(cp, *outbuf, be);
      *outbuf += 4;
      *outbytesleft -= 4;
      return 0;
    } else {
      int wlen;
      wchar_t wc[2];

      if (u16_enc_len(cp) == 2) {
        u16_2 cu = u16_enc_2(cp);
        wc[0] = cu.cu0;
        wc[1] = cu.cu1;
        wlen = 2;
      } else {
        wc[0] = cp;
        wlen = 1;
      }

      bool page_is_unicode = page_is_u18030(page);
      BOOL used_default = FALSE;
      int n = WideCharToMultiByte(
          page,
          0,
          wc,
          wlen,
          (char *)*outbuf,
          (int)*outbytesleft,
          NULL,
          // microsoft docs:
          //   For the CP_UTF7 and CP_UTF8 settings for CodePage,
          //   this parameter must be set to NULL.
          //   Otherwise, the function fails with ERROR_INVALID_PARAMETER.
          // undocumented:
          //   It must be NULL for 54936 (GB 18030).
          page_is_unicode ? NULL : &used_default);

      if (n == 0) {
        DWORD err = GetLastError();
        if (err == ERROR_INSUFFICIENT_BUFFER) {
          errno = E2BIG;
          return -1;
        }

        errno = EILSEQ;
        return -1;
      }

      if (used_default)
        switch (cd->mode) {
        case error_mode::strict:
          errno = EILSEQ;
          return -1;
        case error_mode::ignore:
          (*non_reversible)++;
          return 0;
        case error_mode::replace:
          (*non_reversible)++;
          *outbuf += n;
          *outbytesleft -= n;
          return 0;
        }

      *outbuf += n;
      *outbytesleft -= n;
      return 0;
    }
  }

  extern "C" size_t iconv(iconv_t cd,
                          const char **inbuf,
                          size_t *inbytesleft,
                          char **outbuf,
                          size_t *outbytesleft)
  {
    if (cd == (iconv_t)-1) {
      errno = EBADF;
      return (size_t)-1;
    }

    if (!inbuf || !*inbuf)
      return 0;

    if (!outbuf || !*outbuf || !outbytesleft) {
      errno = EINVAL;
      return (size_t)-1;
    }

    size_t non_reversible = 0;

    while (*inbytesleft > 0) {
      decode_result dr =
          decode_one(cd->from_page, (const uint8_t *)*inbuf, *inbytesleft);

      if (!dr.ok) {
        if (dr.incomplete) {
          errno = EILSEQ;
          return (size_t)-1;
        }
        switch (cd->mode) {
        case error_mode::strict:
          errno = EILSEQ;
          return (size_t)-1;
        case error_mode::ignore:
          non_reversible++;
          *inbuf += dr.advance;
          *inbytesleft -= dr.advance;
          continue;
        case error_mode::replace:
          non_reversible++;
          dr.codepoint = cp_rep;
          // fall through
        }
      }

      int result = encode_one(
          cd, dr.codepoint, (uint8_t **)outbuf, outbytesleft, &non_reversible);
      if (result < 0)
        return (size_t)-1;

      *inbuf += dr.advance;
      *inbytesleft -= dr.advance;
    }

    return non_reversible;
  }
} // namespace libiconv
