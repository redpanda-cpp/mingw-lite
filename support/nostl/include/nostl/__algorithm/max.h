#pragma once

#include "../__config.h"

namespace NS_NOSTL
{
  template <class T>
  constexpr const T &max(const T &a, const T &b)
  {
    return (a < b) ? b : a;
  }

  template <class T, class Compare>
  constexpr const T &max(const T &a, const T &b, Compare comp)
  {
    return comp(a, b) ? b : a;
  }
} // namespace NS_NOSTL
