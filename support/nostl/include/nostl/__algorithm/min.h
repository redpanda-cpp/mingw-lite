#pragma once

#include "../__config.h"

namespace NS_NOSTL
{
  template <class T>
  constexpr const T &min(const T &a, const T &b)
  {
    return (b < a) ? b : a;
  }

  template <class T, class Compare>
  constexpr const T &min(const T &a, const T &b, Compare comp)
  {
    return comp(b, a) ? b : a;
  }
} // namespace NS_NOSTL
