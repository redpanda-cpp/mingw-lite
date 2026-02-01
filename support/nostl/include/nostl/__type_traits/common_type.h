#pragma once

#include "../__config.h"

#include "../__utility/declval.h"
#include "conditional.h"
#include "decay.h"
#include "is_same.h"
#include "void_t.h"

namespace NS_NOSTL
{
  template <class... T>
  struct common_type;

  template <class... T>
  using common_type_t = typename common_type<T...>::type;

  // sizeof...(T) == 0
  template <class...>
  struct common_type
  {
  };

  // sizeof...(T) == 1
  template <class T>
  struct common_type<T>
  {
    using type = T;
  };

  namespace detail
  {
    template <class T1, class T2>
    using conditional_result_t =
        decltype(false ? declval<T1>() : declval<T2>());

    template <class, class, class = void>
    struct decay_conditional_result
    {
    };

    template <class T1, class T2>
    struct decay_conditional_result<T1,
                                    T2,
                                    void_t<conditional_result_t<T1, T2>>>
        : decay<conditional_result_t<T1, T2>>
    {
    };

    template <class T1, class T2, class = void>
    struct common_type_2_impl : decay_conditional_result<const T1 &, const T2 &>
    {
    };

    template <class T1, class T2>
    struct common_type_2_impl<T1, T2, void_t<conditional_result_t<T1, T2>>>
        : decay_conditional_result<T1, T2>
    {
    };
  } // namespace detail

  // sizeof...(T) == 2
  template <class T1, class T2>
  struct common_type<T1, T2>
      : conditional_t<is_same_v<T1, decay_t<T1>> && is_same_v<T2, decay_t<T2>>,
                      detail::common_type_2_impl<T1, T2>,
                      common_type<decay_t<T1>, decay_t<T2>>>
  {
  };

  namespace detail
  {
    template <class AlwaysVoid, class T1, class T2, class... R>
    struct common_type_multi_impl
    {
    };

    template <class T1, class T2, class... R>
    struct common_type_multi_impl<void_t<typename common_type<T1, T2>::type>,
                                  T1,
                                  T2,
                                  R...>
        : common_type<typename common_type<T1, T2>::type, R...>
    {
    };
  } // namespace detail

  // sizeof...(T) > 2
  template <class T1, class T2, class... R>
  struct common_type<T1, T2, R...>
      : detail::common_type_multi_impl<void, T1, T2, R...>
  {
  };
} // namespace NS_NOSTL
