#pragma once

#include "../__config.h"

namespace NS_NOSTL
{
  template <class T>
  struct make_signed;

  template <class T>
  using make_signed_t = typename make_signed<T>::type;

  namespace detail
  {
    // TODO: support enum
    template <class T>
    struct make_signed_impl;

    template <class T>
    struct make_signed_impl<const T>
    {
      using type = const make_signed_t<T>;
    };

    template <class T>
    struct make_signed_impl<volatile T>
    {
      using type = volatile make_signed_t<T>;
    };

    template <class T>
    struct make_signed_impl<const volatile T>
    {
      using type = const volatile make_signed_t<T>;
    };

    template <>
    struct make_signed_impl<char>
    {
      using type = signed char;
    };

    template <>
    struct make_signed_impl<signed char>
    {
      using type = signed char;
    };

    template <>
    struct make_signed_impl<unsigned char>
    {
      using type = signed char;
    };

    template <>
    struct make_signed_impl<signed short>
    {
      using type = signed short;
    };

    template <>
    struct make_signed_impl<unsigned short>
    {
      using type = signed short;
    };

    template <>
    struct make_signed_impl<signed int>
    {
      using type = signed int;
    };

    template <>
    struct make_signed_impl<unsigned int>
    {
      using type = signed int;
    };

    template <>
    struct make_signed_impl<signed long>
    {
      using type = signed long;
    };

    template <>
    struct make_signed_impl<unsigned long>
    {
      using type = signed long;
    };

    template <>
    struct make_signed_impl<signed long long>
    {
      using type = signed long long;
    };

    template <>
    struct make_signed_impl<unsigned long long>
    {
      using type = signed long long;
    };
  } // namespace detail

  template <class T>
  struct make_signed
  {
    using type = typename detail::make_signed_impl<T>::type;
  };
} // namespace NS_NOSTL
