#pragma once

#include <thunk/os.h>

#include <stdint.h>

#include <errhandlingapi.h>
#include <synchapi.h>

namespace mingw_thunk::internal
{
  // https://fanael.github.io/stockfish-on-windows-98.html

  struct win9x_critical_section
  {
    uint8_t type; // Always 4.
    struct win9x_critical_section_impl *impl;
    // Padding for NT compatibility, where the structure is completely
    // different.
    uint32_t reserved[4];

    bool check_type() const
    {
      if (type != 4) [[unlikely]] {
        RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, 0);
        return false;
      }
      return true;
    }
  };
  static_assert(sizeof(win9x_critical_section) == sizeof(CRITICAL_SECTION));

  struct win9x_critical_section_impl
  {
    uint8_t type;
    int recursion_count; // we do not care about it
                         // just make kernel32 version happy
    uintptr_t owner_thread;
    uint32_t reserved;
    int lock_count; // 1 - not locked
                    // 0 - locked by owner_thread
                    // <0 - recursively locked
    void *internal_pointers[3];

    bool try_enter(uintptr_t current_tdbx)
    {
      int expected_lock_count = 1;
      if (__atomic_compare_exchange_n(&lock_count,
                                      &expected_lock_count,
                                      0,
                                      false,
                                      __ATOMIC_ACQUIRE,
                                      __ATOMIC_RELAXED)) {
        owner_thread = current_tdbx;
        ++recursion_count;
        return true;
      } else if (owner_thread == current_tdbx) {
        __atomic_sub_fetch(&lock_count, 1, __ATOMIC_RELAXED);
        ++recursion_count;
        return true;
      } else {
        return false;
      }
    }

    void leave()
    {
      // From Microsoft documentation for LeaveCriticalSection:
      // If a thread calls LeaveCriticalSection when it does not have ownership
      // of the specified critical section object, an error occurs that may
      // cause another thread using EnterCriticalSection to wait indefinitely.

      // okay, don't check ownership.

      --recursion_count;
      __atomic_add_fetch(&lock_count, 1, __ATOMIC_RELEASE);
    }
  };
  static_assert(sizeof(win9x_critical_section_impl) == 32);

  inline ptrdiff_t win9x_tdbx_offset()
  {
    // 95, 98: 0x50; Me: 0x80
    static const ptrdiff_t offset =
        os_version().dwMinorVersion > 10 ? 0x80 : 0x50;
    return offset;
  }

  inline uintptr_t win9x_current_tdbx()
  {
    struct _TEB *teb = NtCurrentTeb();
    return *reinterpret_cast<uintptr_t *>(uintptr_t(teb) +
                                          internal::win9x_tdbx_offset());
  }

} // namespace mingw_thunk::internal
