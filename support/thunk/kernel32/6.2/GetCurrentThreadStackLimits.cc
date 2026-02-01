#include <thunk/_common.h>
#include <thunk/os.h>

#include <stdint.h>

#include <windows.h>

namespace mingw_thunk
{
#if defined(__x86_64__)
  constexpr ptrdiff_t high_limit_offset = 8;
  constexpr ptrdiff_t low_limit_offset = 5240;
#elif defined(__i386__)
  constexpr ptrdiff_t high_limit_offset = 4;
  constexpr ptrdiff_t low_limit_offset = 3596;
  constexpr ptrdiff_t win9x_limit_offset = 8;
#else
#error "Unsupported architecture"
#endif

  __DEFINE_THUNK(kernel32,
                 8,
                 VOID,
                 WINAPI,
                 GetCurrentThreadStackLimits,
                 _Out_ PULONG_PTR LowLimit,
                 _Out_ PULONG_PTR HighLimit)
  {
    if (const auto pfn = try_get_GetCurrentThreadStackLimits())
      return pfn(LowLimit, HighLimit);

    void *teb = NtCurrentTeb();

#if THUNK_LEVEL >= NTDDI_WIN4

    *HighLimit = *(uintptr_t *)((uintptr_t)teb + high_limit_offset);
    *LowLimit = *(uintptr_t *)((uintptr_t)teb + low_limit_offset);
    return;

#else

    if (internal::is_nt()) {
      *HighLimit = *(uintptr_t *)((uintptr_t)teb + high_limit_offset);
      *LowLimit = *(uintptr_t *)((uintptr_t)teb + low_limit_offset);
    } else {
      uintptr_t stack_base = *(uintptr_t *)((uintptr_t)teb + high_limit_offset);
      uintptr_t stack_limit =
          *(uintptr_t *)((uintptr_t)teb + win9x_limit_offset);
      // report at least 64 KiB stack
      if (stack_base - stack_limit < 0x10000)
        stack_limit = stack_base - 0x10000;
      *HighLimit = stack_base;
      *LowLimit = stack_limit;
    }
    return;

#endif
  }
} // namespace mingw_thunk
