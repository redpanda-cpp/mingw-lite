#include <thunk/_common.h>

#include <cpuid.h>
#include <stdint.h>

#include <windows.h>

namespace mingw_thunk
{
  namespace internal
  {

    // clang-format off
    /*
      By executing the command:
        diff <(i686-w64-mingw32-g++ -dM -E -march=i486 - </dev/null | sort) <(i686-w64-mingw32-g++ -dM -E -march=i586 - </dev/null | sort)
      we get:
        200c200
        < #define __GCC_ATOMIC_LLONG_LOCK_FREE 1
        ---
        > #define __GCC_ATOMIC_LLONG_LOCK_FREE 2
        211a212
        > #define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8 1
        227,228c228,229
        < #define __i486 1
        < #define __i486__ 1
        ---
        > #define __i586 1
        > #define __i586__ 1
        303a305,306
        > #define __pentium 1
        > #define __pentium__ 1
        346c349,350
        < #define __tune_i486__ 1
        ---
        > #define __tune_i586__ 1
        > #define __tune_pentium__ 1

      The only way to detect target >= i586 (pentium) is to check for
        __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8
      or
        __GCC_ATOMIC_LLONG_LOCK_FREE
    */
    // clang-format on

#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8) // i586 or later (see above)

    constexpr bool is_cpuid_implemented()
    {
      return true;
    }

#else

    static bool is_cpuid_implemented()
    {
      static bool result = []() {
        constexpr uint32_t cpuid_bit = 1 << 21;
        uint32_t original = __builtin_ia32_readeflags_u32();

        // try invert
        __builtin_ia32_writeeflags_u32(original ^ cpuid_bit);
        uint32_t updated = __builtin_ia32_readeflags_u32();

        // is inverted?
        return bool((original ^ updated) & cpuid_bit);
      }();

      return result;
    }

#endif

  } // namespace internal

  __DEFINE_THUNK(kernel32, 0, DWORD, WINAPI, GetCurrentProcessorNumber)
  {
    if (const auto pfn = try_get_GetCurrentProcessorNumber())
      return pfn();

    if (!internal::is_cpuid_implemented())
      return 0;

    // Windows < NT 5.2 is x86-32 only (stop reviving IA-64) and supports no
    // more than 32 logical processors (according to Task Manager Processor
    // Affinity dialog). No more checks required.
    uint32_t eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    return ebx >> 24;
  }
} // namespace mingw_thunk
