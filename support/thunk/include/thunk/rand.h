#pragma once

#include <stdint.h>
#include <sysinfoapi.h>

namespace mingw_thunk
{
  namespace i
  {
    inline uint64_t &__rand_seed()
    {
      static uint64_t seed = GetTickCount64();
      return seed;
    }

    inline uint64_t __rand64()
    {
      uint64_t &seed = __rand_seed();
      uint64_t old_seed = __atomic_load_n(&seed, __ATOMIC_ACQUIRE);
      uint64_t new_seed;
      do {
        new_seed = 6364136223846793005ull * old_seed + 1; // musl's choice
      } while (!__atomic_compare_exchange_n(&seed,
                                            &old_seed,
                                            new_seed,
                                            false,
                                            __ATOMIC_RELEASE,
                                            __ATOMIC_ACQUIRE));
      return new_seed;
    }

    inline uint32_t __rand32()
    {
      return __rand64() >> 32;
    }
  } // namespace i

  namespace c
  {
    inline int rand()
    {
      return i::__rand64() >> (64 - 15);
    }

    inline void srand(unsigned s)
    {
      uint64_t &seed = i::__rand_seed();
      uint64_t new_seed = s - 1; // musl's choice
      __atomic_store(&seed, &new_seed, __ATOMIC_RELEASE);
    }
  } // namespace c
} // namespace mingw_thunk
