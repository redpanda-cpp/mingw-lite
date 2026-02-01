#pragma once

#include <stddef.h>

#include "../__config.h"

#include "../__type_traits/common_type.h"
#include "../__type_traits/make_signed.h"

namespace NS_NOSTL
{
  template <class C>
  constexpr auto size(C &c) -> decltype(c.size())
  {
    return c.size();
  }

  template <class C>
  constexpr auto ssize(const C &c)
      -> common_type_t<ptrdiff_t, make_signed_t<decltype(c.size())>>
  {
    return c.size();
  }

  template <class T, size_t N>
  constexpr size_t size(const T (&)[N]) noexcept
  {
    return N;
  }

  template <class T, ptrdiff_t N>
  constexpr ptrdiff_t ssize(const T (&)[N]) noexcept
  {
    return N;
  }
} // namespace NS_NOSTL
