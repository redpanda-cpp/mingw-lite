#pragma once

#include <windows.h>

namespace intl
{
  inline bool system_is_utf8()
  {
    return GetACP() == CP_UTF8;
  }
} // namespace intl
