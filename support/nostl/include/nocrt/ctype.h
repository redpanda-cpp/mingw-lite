#pragma once

#include "__config.h"

namespace NS_NOCRT
{
  inline int isalpha(int c)
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }
} // namespace NS_NOCRT
