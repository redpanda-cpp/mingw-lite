#pragma once

#include <stdint.h>

#include "encoding.h"

namespace libiconv
{
  struct u16_2
  {
    uint16_t cu0;
    uint16_t cu1;
  };

  inline bool page_is_u8(code_page_t page)
  {
    return page == 65001;
  }

  inline bool page_is_u16(code_page_t page)
  {
    return page == 1200 || page == 1201;
  }

  inline bool page_is_u32(code_page_t page)
  {
    return page == 12000 || page == 12001;
  }

  inline bool page_is_be(code_page_t page)
  {
    return page == 1201 || page == 12001;
  }

  inline bool page_is_le(code_page_t page)
  {
    return page == 1200 || page == 12000;
  }

  inline bool page_is_u18030(code_page_t page)
  {
    return page == 54936;
  }

  inline bool cp_is_surrogate(uint32_t cu)
  {
    return (cu >= 0xD800 && cu <= 0xDFFF);
  }

  inline bool cp_not_character(uint32_t cp)
  {
    return (cp >= 0xFDD0 && cp <= 0xFDEF) || (cp & 0xFFFF) == 0xFFFE ||
           (cp & 0xFFFF) == 0xFFFF;
  }

  inline bool cp_out_of_range(uint32_t cp)
  {
    return cp > 0x10FFFF;
  }

  struct u8_2
  {
    uint8_t b0;
    uint8_t b1;
  };

  struct u8_3
  {
    uint8_t b0;
    uint8_t b1;
    uint8_t b2;
  };

  struct u8_4
  {
    uint8_t b0;
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
  };

  inline int u8_dec_len(uint8_t b)
  {
    if ((b & 0b1000'0000) == 0)
      return 1;
    else if ((b & 0b1110'0000) == 0b1100'0000)
      return 2;
    else if ((b & 0b1111'0000) == 0b1110'0000)
      return 3;
    else if ((b & 0b1111'1000) == 0b1111'0000)
      return 4;
    else
      return -1;
  }

  inline int u8_enc_len(uint32_t cp)
  {
    if (cp < 0x80)
      return 1;
    else if (cp < 0x800)
      return 2;
    else if (cp < 0x10000)
      return 3;
    else
      return 4;
  }

  inline uint32_t u8_dec(uint8_t b0, uint8_t b1)
  {
    return ((uint32_t)(b0 & 0x1F) << 6) | (b1 & 0x3F);
  }

  inline uint32_t u8_dec(uint8_t b0, uint8_t b1, uint8_t b2)
  {
    return ((uint32_t)(b0 & 0x0F) << 12) | ((uint32_t)(b1 & 0x3F) << 6) |
           (b2 & 0x3F);
  }

  inline uint32_t u8_dec(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
  {
    return ((uint32_t)(b0 & 0x07) << 18) | ((uint32_t)(b1 & 0x3F) << 12) |
           ((uint32_t)(b2 & 0x3F) << 6) | (b3 & 0x3F);
  }

  inline u8_2 u8_enc_2(uint32_t cp)
  {
    return {
        (uint8_t)(0xC0 | (cp >> 6)),
        (uint8_t)(0x80 | (cp & 0x3F)),
    };
  }

  inline u8_3 u8_enc_3(uint32_t cp)
  {
    return {
        (uint8_t)(0xE0 | (cp >> 12)),
        (uint8_t)(0x80 | ((cp >> 6) & 0x3F)),
        (uint8_t)(0x80 | (cp & 0x3F)),
    };
  }

  inline u8_4 u8_enc_4(uint32_t cp)
  {
    return {
        (uint8_t)(0xF0 | (cp >> 18)),
        (uint8_t)(0x80 | ((cp >> 12) & 0x3F)),
        (uint8_t)(0x80 | ((cp >> 6) & 0x3F)),
        (uint8_t)(0x80 | (cp & 0x3F)),
    };
  }

  inline uint32_t u16_dec(uint16_t cu0, uint16_t cu1)
  {
    uint32_t hi_off = cu0 - 0xD800;
    uint32_t lo_off = cu1 - 0xDC00;
    uint32_t cp = 0x10000 + (hi_off << 10) + lo_off;
    return cp;
  }

  inline int u16_enc_len(uint32_t cp)
  {
    return cp >= 0x10000 ? 2 : 1;
  }

  inline u16_2 u16_enc_2(uint32_t cp)
  {
    uint32_t off = cp - 0x10000;
    uint16_t hi_off = off >> 10;
    uint16_t lo_off = off & 0x3FF;
    uint16_t cu0 = 0xD800 + hi_off;
    uint16_t cu1 = 0xDC00 + lo_off;
    return {cu0, cu1};
  }

  inline bool u16_is_surrogate_hi(uint16_t cu)
  {
    return (cu >= 0xD800 && cu <= 0xDBFF);
  }

  inline bool u16_is_surrogate_lo(uint16_t cu)
  {
    return (cu >= 0xDC00 && cu <= 0xDFFF);
  }

  inline int u18030_dec_len(uint8_t b0)
  {
    if (b0 <= 0x7f)
      return 1;
    else if (b0 >= 0x80 && b0 <= 0xfe)
      return 2;
    else
      return 1;
  }

  inline int u18030_dec_len(uint8_t b0, uint8_t b1)
  {
    if (b0 <= 0x7f)
      return 1;
    else if (b0 >= 0x80 && b0 <= 0xfe) {
      if (b1 >= 0x30 && b1 <= 0x39)
        return 4;
      else if (b1 >= 0x40 && b1 <= 0xfe && b1 != 0x7f)
        return 2;
      else
        return 1;
    } else
      return 1;
  }

  constexpr uint32_t cp_rep = 0xFFFD;

} // namespace libiconv
