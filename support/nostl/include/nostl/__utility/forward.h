#pragma once

#include "../__config.h"

#include "../__type_traits/remove_reference.h"

namespace NS_NOSTL
{
  template <class T>
  constexpr T &&forward(remove_reference_t<T> &t) noexcept
  {
    return static_cast<T &&>(t);
  }

  template <class T>
  constexpr T &&forward(remove_reference_t<T> &&t) noexcept
  {
    return static_cast<T &&>(t);
  }
} // namespace NS_NOSTL
