#pragma once

#include "../__config.h"

namespace NS_NOSTL
{
  template <bool B, class T, class F>
  struct conditional;

  template <bool B, class T, class F>
  using conditional_t = typename conditional<B, T, F>::type;

  template <bool B, class T, class F>
  struct conditional
  {
    using type = T;
  };

  template <class T, class F>
  struct conditional<false, T, F>
  {
    using type = F;
  };
} // namespace NS_NOSTL
