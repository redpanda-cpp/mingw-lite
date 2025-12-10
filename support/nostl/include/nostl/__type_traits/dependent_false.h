#pragma once

#include "../__config.h"

namespace NS_NOSTL
{
  template <class...>
  constexpr bool dependent_false = false;
} // namespace NS_NOSTL
