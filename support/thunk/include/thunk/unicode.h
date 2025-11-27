#pragma once

#include <stdint.h>

namespace mingw_thunk::internal
{
  struct u16_2
  {
    char16_t high;
    char16_t low;
  };

  constexpr bool u16_is_high(char16_t c)
  {
    return c >= 0xd800 && c < 0xdc00;
  }

  constexpr bool u16_is_low(char16_t c)
  {
    return c >= 0xdc00 && c < 0xe000;
  }

  constexpr int u16_dec_2(u16_2 s)
  {
    const char16_t high = s.high - 0xd800;
    const char16_t low = s.low - 0xdc00;
    return high * 0x400 + low + 0x10000;
  }

  constexpr int u16_enc_len(char32_t ch)
  {
    if (ch < 0x10000)
      return 1;
    else
      return 2;
  }

  constexpr u16_2 u16_enc_2(char32_t ch)
  {
    const char16_t high = (ch - 0x10000) / 0x400 + 0xd800;
    const char16_t low = (ch - 0x10000) % 0x400 + 0xdc00;
    return {high, low};
  }

  constexpr char16_t u16_rep = 0xfffd;
} // namespace mingw_thunk::internal

namespace mingw_thunk::internal
{
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

  constexpr int u8_dec_len(uint8_t b)
  {
    if ((b & 0b1000'0000) == 0)
      return 1;
    if ((b & 0b1110'0000) == 0b1100'0000)
      return 2;
    if ((b & 0b1111'0000) == 0b1110'0000)
      return 3;
    if ((b & 0b1111'1000) == 0b1111'0000)
      return 4;
    return 0;
  }

  constexpr bool u8_is_trail(uint8_t b)
  {
    return b >= 0b1000'0000 && b < 0b1100'0000;
  }

  constexpr char32_t u8_dec_2(u8_2 s)
  {
    uint8_t b0 = s.b0 & 0b0001'1111;
    uint8_t b1 = s.b1 & 0b0011'1111;
    return (b0 << 6) | b1;
  }

  constexpr char32_t u8_dec_3(u8_3 s)
  {
    uint8_t b0 = s.b0 & 0b0000'1111;
    uint8_t b1 = s.b1 & 0b0011'1111;
    uint8_t b2 = s.b2 & 0b0011'1111;
    return (b0 << 12) | (b1 << 6) | b2;
  }

  constexpr char32_t u8_dec_4(u8_4 s)
  {
    uint8_t b0 = s.b0 & 0b0000'1111;
    uint8_t b1 = s.b1 & 0b0011'1111;
    uint8_t b2 = s.b2 & 0b0011'1111;
    uint8_t b3 = s.b3 & 0b0011'1111;
    return (b0 << 18) | (b1 << 12) | (b2 << 6) | b3;
  }

  constexpr int u8_enc_len(char32_t ch)
  {
    if (ch < 0x80)
      return 1;
    else if (ch < 0x800)
      return 2;
    else if (ch < 0x10000)
      return 3;
    else
      return 4;
  }

  constexpr u8_2 u8_enc_2(char32_t ch)
  {
    uint8_t b1 = 0b1100'0000 | (ch >> 6);
    uint8_t b2 = 0b1000'0000 | (ch & 0b11'1111);
    return {b1, b2};
  }

  constexpr u8_3 u8_enc_3(char32_t ch)
  {
    uint8_t b1 = 0b1110'0000 | (ch >> 12);
    uint8_t b2 = 0b1000'0000 | ((ch >> 6) & 0b11'1111);
    uint8_t b3 = 0b1000'0000 | (ch & 0b11'1111);
    return {b1, b2, b3};
  }

  constexpr u8_4 u8_enc_4(char32_t ch)
  {
    uint8_t b1 = 0b1111'0000 | (ch >> 18);
    uint8_t b2 = 0b1000'0000 | ((ch >> 12) & 0b11'1111);
    uint8_t b3 = 0b1000'0000 | ((ch >> 6) & 0b11'1111);
    uint8_t b4 = 0b1000'0000 | (ch & 0b11'1111);
    return {b1, b2, b3, b4};
  }

  constexpr u8_3 u8_rep = u8_enc_3(u16_rep);
} // namespace mingw_thunk::internal
