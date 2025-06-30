#pragma once

#include "__config.h"

namespace NS_NOCRT
{
  inline int *__errno_location()
  {
    static thread_local int errno_val = 0;
    return &errno_val;
  }

} // namespace NS_NOCRT

#define NOCRT_errno (*NS_NOCRT::__errno_location())

#ifdef ENOMEM
static_assert(ENOMEM == 12, "ENOMEM != 12");
#else
  #define ENOMEM 12
#endif

#ifdef EINVAL
static_assert(EINVAL == 22, "EINVAL != 22");
#else
  #define EINVAL 22
#endif
