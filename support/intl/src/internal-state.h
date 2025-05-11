#pragma once

#include <memory>

#include <stdlib.h>

namespace intl
{
  template <typename T>
  using c_unique_ptr = std::unique_ptr<T, decltype(&free)>;

  inline c_unique_ptr<char[]> default_domain{nullptr, &free};

  inline c_unique_ptr<char[]> cached_domain{nullptr, &free};
  inline c_unique_ptr<char[]> cached_data{nullptr, &free};
  inline size_t cached_size{0};
} // namespace intl
