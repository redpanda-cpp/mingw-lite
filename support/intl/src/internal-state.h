#pragma once

#include "util/cache_manager_t.h"
#include "util/owning_ptr.h"

namespace intl
{
  extern cache_manager_t cache_manager;
  extern owning_ptr<char[]> default_domain;
} // namespace intl
