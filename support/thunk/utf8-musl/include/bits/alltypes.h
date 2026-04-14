#pragma once

namespace mingw_thunk
{
  namespace musl
  {
#ifdef __NEED_mbstate_t
    struct mbstate_t
    {
      unsigned __opaque1, __opaque2;
    };
#endif
  } // namespace musl
} // namespace mingw_thunk
