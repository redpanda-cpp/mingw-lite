#pragma once

#include <stdint.h>
#include <sysinfoapi.h>

#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8) &&                             \
    __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8
#define _MINGW_THUNK_RAND_TARGET_HAVE_ATOMIC 1
#define _MINGW_THUNK_RAND_ENABLE_ATOMIC
#else
#define _MINGW_THUNK_RAND_TARGET_HAVE_ATOMIC 0
#define _MINGW_THUNK_RAND_ENABLE_ATOMIC __attribute__((__target__("arch=i586")))
#endif

namespace mingw_thunk
{
  namespace g
  {
    inline uint64_t &rand_seed()
    {
      static uint64_t seed = GetTickCount64();
      return seed;
    }

    inline bool &rand_lock()
    {
      static bool lock = false;
      return lock;
    }

  } // namespace g

  namespace i
  {
    _MINGW_THUNK_RAND_ENABLE_ATOMIC
    inline uint64_t rand64_atomic()
    {
      uint64_t &seed = g::rand_seed();
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

    inline uint64_t rand64_lock()
    {
      bool &lock = g::rand_lock();
      while (__atomic_test_and_set(&lock, __ATOMIC_ACQUIRE))
        ;
      uint64_t &seed = g::rand_seed();
      seed = 6364136223846793005ull * seed + 1;
      uint64_t result = seed;
      __atomic_clear(&lock, __ATOMIC_RELEASE);
      return result;
    }

    inline uint64_t rand64()
    {
#if _MINGW_THUNK_RAND_TARGET_HAVE_ATOMIC
      return rand64_atomic();
#else
      static decltype(&rand64) rand64_func = nullptr;
      if (!rand64_func) {
        if (__builtin_cpu_supports("cmpxchg8b"))
          rand64_func = rand64_atomic;
        else
          rand64_func = rand64_lock;
      }
      return rand64_func();
#endif
    }

    inline uint32_t rand32()
    {
      return rand64() >> 32;
    }

    _MINGW_THUNK_RAND_ENABLE_ATOMIC
    inline void srand_atomic(unsigned s)
    {
      uint64_t &seed = g::rand_seed();
      uint64_t new_seed = s - 1; // musl's choice
      __atomic_store(&seed, &new_seed, __ATOMIC_RELEASE);
    }

    inline void srand_lock(unsigned s)
    {
      bool &lock = g::rand_lock();
      while (__atomic_test_and_set(&lock, __ATOMIC_ACQUIRE))
        ;
      g::rand_seed() = s - 1;
      __atomic_clear(&lock, __ATOMIC_RELEASE);
    }

    inline void srand(unsigned s)
    {
#if __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8
      srand_atomic(s);
#else
      static decltype(&srand) srand_func = nullptr;
      if (!srand_func) {
        if (__builtin_cpu_supports("cmpxchg8b"))
          srand_func = srand_atomic;
        else
          srand_func = srand_lock;
      }
      return srand_func(s);
#endif
    }
  } // namespace i

  namespace c
  {
    inline int rand()
    {
      return i::rand64() >> (64 - 15);
    }

    inline void srand(unsigned s)
    {
      i::srand(s);
    }
  } // namespace c
} // namespace mingw_thunk

#undef _MINGW_THUNK_RAND_TARGET_HAVE_ATOMIC
