#pragma once

#include <stddef.h>

#include "../__config.h"

namespace NS_NOSTL
{
  template <class C>
  constexpr auto end(C &c) -> decltype(c.end())
  {
    return c.end();
  }

  template <class C>
  auto end(const C &c) -> decltype(c.end())
  {
    return c.end();
  }

  template <class T, size_t N>
  T *end(T (&array)[N])
  {
    return array + N;
  }

  template <class C>
  constexpr auto cend(const C &c) noexcept -> decltype(end(c))
  {
    return end(c);
  }
} // namespace NS_NOSTL
