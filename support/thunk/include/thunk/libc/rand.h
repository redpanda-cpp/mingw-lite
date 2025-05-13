#pragma once

#include <thunk/stl/mutex.h>

#include <stdint.h>
#include <sysinfoapi.h>

namespace mingw_thunk::internal
{
  inline uint64_t c_rand_seed = GetTickCount64();

  inline uint32_t rand32()
  {
    uint64_t &seed = internal::c_rand_seed;
    seed = 6364136223846793005ULL * seed + 1; // musl's choice
    return seed >> 32;
  }

  inline int rand()
  {
    return internal::rand32() >> 1;
  }

  inline void srand(unsigned s)
  {
    internal::c_rand_seed = s - 1; // musl's choice
  }
} // namespace mingw_thunk::internal
