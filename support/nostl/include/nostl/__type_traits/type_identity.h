#pragma once

#include "../__config.h"

namespace NS_NOSTL
{
  template <class T>
  struct type_identity
  {
    using type = T;
  };

  template <class T>
  using type_identity_t = typename type_identity<T>::type;
} // namespace NS_NOSTL
