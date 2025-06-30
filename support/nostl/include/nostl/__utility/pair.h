#pragma once

#include "../__config.h"

#include "forward.h"

namespace NS_NOSTL
{
  template <class T1, class T2>
  struct pair
  {
  public:
    using first_type = T1;
    using second_type = T2;

  public:
    T1 first;
    T2 second;

  public:
    constexpr pair() : first(), second()
    {
    }

    constexpr pair(const T1 &x, const T2 &y) : first(x), second(y)
    {
    }

    template <class U1 = T1, class U2 = T2>
    constexpr pair(U1 &&x, U2 &&y)
        : first(forward<U1>(x)), second(forward<U2>(y))
    {
    }

    pair(const pair &p) = default;

    pair(pair &&p) = default;

    constexpr pair &operator=(const pair &other) = default;

    constexpr pair &operator=(pair &&other) = default;
  };
} // namespace NS_NOSTL
