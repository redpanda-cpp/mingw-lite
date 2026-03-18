#pragma once

#include "../include/features.h"

#include <stdint.h>

namespace mingw_thunk
{
  namespace musl
  {
    extern hidden const uint32_t bittab[];

    constexpr uint32_t OOB(uint32_t c, uint32_t b)
    {
      return ((((b) >> 3) - 0x10) | (((b) >> 3) + ((int32_t)(c) >> 26))) & ~7;
    }

    constexpr uint32_t R(uint32_t a, uint32_t b)
    {
      return (a == 0x80 ? 0x40u - b : 0u - a) << 23;
    }

    constexpr unsigned SA = 0xc2u;
    constexpr unsigned SB = 0xf4u;

    constexpr int CODEUNIT(unsigned c)
    {
      return 0xdfff & (signed char)(c);
    }

    constexpr bool IS_CODEUNIT(unsigned c)
    {
      return c - 0xdf80 < 0x80;
    }
  } // namespace musl
} // namespace mingw_thunk
