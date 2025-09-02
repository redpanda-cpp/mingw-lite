#pragma once

#include <stddef.h>

#include "../__config.h"

namespace NS_NOSTL
{
  template <class C>
  constexpr auto begin(C &c) -> decltype(c.begin())
  {
    return c.begin();
  }

  template <class C>
  auto begin(const C &c) -> decltype(c.begin())
  {
    return c.begin();
  }

  template <class T, size_t N>
  T *begin(T (&array)[N])
  {
    return array;
  }

  template <class C>
  constexpr auto cbegin(const C &c) noexcept -> decltype(begin(c))
  {
    return begin(c);
  }
} // namespace NS_NOSTL
