#pragma once

#include "../__config.h"

#include "remove_cv.h"
#include "remove_reference.h"

namespace NS_NOSTL
{
  template <class T>
  struct decay;

  template <class T>
  using decay_t = typename decay<T>::type;

  namespace detail
  {
    template <class T>
    struct decay_impl
    {
      using type = remove_cv_t<T>;
    };

    // array
    template <class T, size_t N>
    struct decay_impl<T[N]>
    {
      using type = T *;
    };

    template <class T>
    struct decay_impl<T[]>
    {
      using type = T *;
    };

    // function
    template <class R, class... Ts>
    struct decay_impl<R(Ts...)>
    {
      using type = R (*)(Ts...);
    };

    template <class R, class... Ts>
    struct decay_impl<R(Ts..., ...)>
    {
      using type = R (*)(Ts..., ...);
    };
  } // namespace detail

  template <class T>
  struct decay : detail::decay_impl<remove_reference_t<T>>
  {
  };
} // namespace NS_NOSTL
