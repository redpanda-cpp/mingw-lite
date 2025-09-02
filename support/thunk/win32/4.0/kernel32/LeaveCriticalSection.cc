#include <thunk/_common.h>
#include <thunk/critical_section.h>
#include <thunk/os.h>

#include <synchapi.h>

#include <stdint.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 VOID,
                 WINAPI,
                 LeaveCriticalSection,
                 _Inout_ LPCRITICAL_SECTION lpCriticalSection)
  {
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)

    return get_LeaveCriticalSection()(lpCriticalSection);

#else

    if (internal::is_nt())
      return get_LeaveCriticalSection()(lpCriticalSection);

    // atomic operations are emulated.
    // TryEnterCriticalSection depends on libatomic's emulated subroutine,
    // so we have to use libatomic also in LeaveCriticalSection.
    const auto *cs =
        reinterpret_cast<internal::win9x_critical_section *>(lpCriticalSection);
    if (!cs->check_type())
      return;

    cs->impl->leave();

#endif
  }
} // namespace mingw_thunk
