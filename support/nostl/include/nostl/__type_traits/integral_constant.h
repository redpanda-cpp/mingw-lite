#pragma once

#include "../__config.h"

namespace NS_NOSTL
{
  template <class T, T v>
  struct integral_constant;

  template <bool B>
  using bool_constant = integral_constant<bool, B>;

  using true_type = bool_constant<true>;

  using false_type = bool_constant<false>;

  template <class T, T v>
  struct integral_constant
  {
    using value_type = T;
    using type = integral_constant<T, v>;

    static constexpr T value = v;

    constexpr operator value_type() const noexcept
    {
      return value;
    }

    constexpr value_type operator()() const noexcept
    {
      return value;
    }
  };
} // namespace NS_NOSTL
