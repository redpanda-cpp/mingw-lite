#pragma once

#include "../__config.h"

#include "type_identity.h"

namespace NS_NOSTL
{
  namespace detail
  {
    template <typename T>
    auto try_add_lvalue_reference(int) -> type_identity<T &>;

    template <typename T>
    auto try_add_lvalue_reference(...) -> type_identity<T>;
  } // namespace detail

  template <class T>
  struct add_lvalue_reference : decltype(detail::try_add_lvalue_reference<T>(0))
  {
  };

  template <class T>
  using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

  namespace detail
  {
    template <typename T>
    auto try_add_rvalue_reference(int) -> type_identity<T &&>;

    template <typename T>
    auto try_add_rvalue_reference(...) -> type_identity<T>;
  } // namespace detail

  template <class T>
  struct add_rvalue_reference : decltype(detail::try_add_rvalue_reference<T>(0))
  {
  };

  template <class T>
  using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
} // namespace NS_NOSTL
