#pragma once

#include "../__config.h"

namespace NS_NOSTL
{
  template <bool B, class T = void>
  struct enable_if
  {
  };

  template <class T>
  struct enable_if<true, T>
  {
    typedef T type;
  };

  template <bool B, class T = void>
  using enable_if_t = typename enable_if<B, T>::type;
} // namespace NS_NOSTL
