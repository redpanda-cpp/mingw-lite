#pragma once

namespace intl
{
  template <class ForwardIt, class T, class Compare>
  constexpr ForwardIt
  lower_bound(ForwardIt first, ForwardIt last, const T &value, Compare comp)
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
} // namespace intl
