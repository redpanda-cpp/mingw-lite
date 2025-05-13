#pragma once

namespace mingw_thunk::internal
{
  template <typename T>
  constexpr const T &min(const T &a, const T &b) noexcept
  {
    return b < a ? b : a;
  }

  template <typename T>
  constexpr const T &max(const T &a, const T &b) noexcept
  {
    return a < b ? b : a;
  }

  template <typename RandomAccessIt, typename T, typename Compare>
  RandomAccessIt lower_bound(RandomAccessIt first,
                             RandomAccessIt last,
                             const T &value,
                             Compare comp)
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
} // namespace mingw_thunk::internal
