#pragma once

#include "../__config.h"

#include "../__functional/less.h"
#include "../__iterator/iterator_traits.h"

namespace NS_NOSTL
{
  namespace detail
  {
    template <class RandomAccessIt, class T, class Compare>
    constexpr RandomAccessIt lower_bound_impl(RandomAccessIt first,
                                              RandomAccessIt last,
                                              const T &value,
                                              Compare comp,
                                              random_access_iterator_tag)
    {
      auto len = last - first;
      while (len != 0) {
        auto l2 = len / 2;
        auto m = first + l2;
        if (comp(*m, value)) {
          first = ++m;
          len -= l2 + 1;
        } else {
          len = l2;
        }
      }
      return first;
    }
  } // namespace detail

  template <class ForwardIt, class T, class Compare>
  constexpr ForwardIt
  lower_bound(ForwardIt first, ForwardIt last, const T &value, Compare comp)
  {
    return detail::lower_bound_impl(
        first,
        last,
        value,
        comp,
        typename iterator_traits<ForwardIt>::iterator_category{});
  }

  template <class ForwardIt, class T>
  constexpr ForwardIt
  lower_bound(ForwardIt first, ForwardIt last, const T &value)
  {
    return lower_bound(first, last, value, less<T>());
  }
} // namespace NS_NOSTL
