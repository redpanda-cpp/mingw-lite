#pragma once

#include "../__config.h"

#include "integral_constant.h"

namespace NS_NOSTL
{
  template <class T>
  struct is_const;

  template <class T>
  constexpr bool is_const_v = is_const<T>::value;

  template <class T>
  struct is_const : false_type
  {
  };

  template <class T>
  struct is_const<const T> : true_type
  {
  };
} // namespace NS_NOSTL
