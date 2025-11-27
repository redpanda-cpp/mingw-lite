#pragma once

#include <thunk/os.h>

#include <stdint.h>

#include <errhandlingapi.h>
#include <synchapi.h>

namespace mingw_thunk::internal
{
  // see also: https://fanael.github.io/stockfish-on-windows-98.html

  constexpr int critical_section_os_minor_win95 = 0;
  constexpr int critical_section_os_minor_win98 = 10;
  constexpr int critical_section_os_minor_winme = 90;
  constexpr int critical_section_os_minor_invalid = -1;

  inline int critical_section_os_minor_init()
  {
    switch (os_version().dwMinorVersion) {
    case critical_section_os_minor_win95:
      return critical_section_os_minor_win95;
    case critical_section_os_minor_win98:
      return critical_section_os_minor_win98;
    case critical_section_os_minor_winme:
      return critical_section_os_minor_winme;
    default:
      RaiseException(STATUS_ASSERTION_FAILURE, 0, 0, 0);
      __builtin_unreachable();
    }
  }

  inline int critical_section_os_minor()
  {
    static const int result = critical_section_os_minor_init();
    return result;
  }

  struct critical_section_impl;

  struct critical_section_t
  {
    union
    {
      uint32_t win95;
      uint8_t win98_winme;
    } type;
    critical_section_impl *impl_win98_winme;
    uint32_t dw2;
    critical_section_impl *impl_win95;
    uint32_t dw4;
    uint32_t dw5;

    bool check_type() const
    {
      bool ok = critical_section_os_minor() == critical_section_os_minor_win95
                    ? type.win95 == 4
                    : type.win98_winme == 4;
      if (!ok) [[unlikely]] {
        RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, 0);
        return false;
      }
      return true;
    }

    critical_section_impl *impl() const
    {
      static critical_section_impl *critical_section_t::*pimpl =
          critical_section_os_minor() == critical_section_os_minor_win95
              ? &critical_section_t::impl_win95
              : &critical_section_t::impl_win98_winme;
      return this->*pimpl;
    }
  };
  static_assert(sizeof(critical_section_t) == sizeof(CRITICAL_SECTION));

  struct critical_section_impl
  {
    uint32_t type;
    int recursion_count;
    uintptr_t owner_thread;
    uint32_t reserved;
    int lock_count; // 1 for unlocked, decreasing
    void *internal_pointers[3];

    [[maybe_unused]]
    void enter()
    {
      void P(critical_section_impl *);

      // kernel32.dll logic for reference
      uintptr_t thread = current_thread();
      if (lock_count-- == 1) {
        // success
        owner_thread = thread;
        ++recursion_count;
        return;
      } else if (owner_thread == thread) {
        // already locked by us
        ++recursion_count;
        return;
      } else {
        // wait for signal from the owner
        P(this);
      }
    }

    [[maybe_unused]]
    void leave()
    {
      void V(critical_section_impl *);

      // kernel32.dll logic for reference
      if (recursion_count-- == 1) {
        // unlocked
        owner_thread = 0;
        ++lock_count;
        if (++lock_count <= 0) {
          // another thread is waiting
          V(this);
        }
      } else {
        // recursive
        ++lock_count;
      }
    }

    bool try_enter()
    {
      uintptr_t thread = current_thread();
      int expected_lock_count = 1;
      if (__atomic_compare_exchange_n(&lock_count,
                                      &expected_lock_count,
                                      0,
                                      false,
                                      __ATOMIC_ACQUIRE,
                                      __ATOMIC_RELAXED)) {
        // success
        owner_thread = thread;
        ++recursion_count;
        return true;
      } else if (owner_thread == thread) {
        // already locked by us
        --lock_count;
        ++recursion_count;
        return true;
      } else {
        return false;
      }
    }

    uintptr_t current_thread()
    {
      static const ptrdiff_t tdbx_offset =
          critical_section_os_minor() == critical_section_os_minor_winme ? 0x80
                                                                         : 0x50;

      struct _TEB *teb = NtCurrentTeb();
      return *reinterpret_cast<uintptr_t *>(uintptr_t(teb) + tdbx_offset);
    }
  };
  static_assert(sizeof(critical_section_impl) == 32);
} // namespace mingw_thunk::internal
