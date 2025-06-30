#pragma once

#include "../__config.h"

#include "../__type_traits/add_reference.h"

namespace NS_NOSTL
{
  template <class T>
  add_rvalue_reference_t<T> declval() noexcept
  {
    static_assert(false, "declval not allowed in an evaluated context");
  }
} // namespace NS_NOSTL
