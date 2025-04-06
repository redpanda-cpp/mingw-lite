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
} // namespace mingw_thunk::internal
