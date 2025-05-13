#pragma once

#include <stddef.h>

namespace mingw_thunk::internal
{
  template <typename T, size_t N>
  constexpr T *begin(T (&array)[N]) noexcept
  {
    return array;
  }

  template <typename T, size_t N>
  constexpr const T *cbegin(const T (&array)[N]) noexcept
  {
    return array;
  }

  template <typename T, size_t N>
  constexpr const T *cend(const T (&array)[N]) noexcept
  {
    return array + N;
  }

  template <typename T, size_t N>
  constexpr T *end(T (&array)[N]) noexcept
  {
    return array + N;
  }

  template <typename T, size_t N>
  constexpr size_t size(const T (&)[N]) noexcept
  {
    return N;
  }
} // namespace mingw_thunk::internal
