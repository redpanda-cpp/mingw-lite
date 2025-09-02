#pragma once

#include "../__config.h"

#include "integral_constant.h"

namespace NS_NOSTL
{
  template <class T, class U>
  struct is_same;

  template <class T, class U>
  constexpr bool is_same_v = is_same<T, U>::value;

  template <class T, class U>
  struct is_same : false_type
  {
  };

  template <class T>
  struct is_same<T, T> : true_type
  {
  };
} // namespace NS_NOSTL
