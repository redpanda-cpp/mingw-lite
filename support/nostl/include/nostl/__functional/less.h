#pragma once

#include "../__config.h"

#include "../__utility/forward.h"

namespace NS_NOSTL
{
  template <class T = void>
  struct less
  {
    constexpr bool operator()(const T &lhs, const T &rhs) const
    {
      return lhs < rhs;
    }
  };

  template <>
  struct less<void>
  {
    template <class T, class U>
    constexpr auto operator()(T &&lhs, U &&rhs) const
        -> decltype(NS_NOSTL::forward<T>(lhs) < NS_NOSTL::forward<U>(rhs))
    {
      return NS_NOSTL::forward<T>(lhs) < NS_NOSTL::forward<U>(rhs);
    }
  };
} // namespace NS_NOSTL
