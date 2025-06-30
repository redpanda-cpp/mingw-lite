#pragma once

#include "../__config.h"

#include "../__type_traits/remove_reference.h"

namespace NS_NOSTL
{
  template <class T>
  constexpr remove_reference_t<T> &&move(T &&t) noexcept
  {
    return static_cast<remove_reference_t<T> &&>(t);
  }
} // namespace NS_NOSTL
