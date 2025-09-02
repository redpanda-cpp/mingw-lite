#include <thunk/_common.h>
#include <thunk/critical_section.h>
#include <thunk/os.h>

#include <synchapi.h>

#include <stdint.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 TryEnterCriticalSection,
                 _Inout_ LPCRITICAL_SECTION lpCriticalSection)
  {
    if (internal::is_nt())
      return get_TryEnterCriticalSection()(lpCriticalSection);

    const auto *cs =
        reinterpret_cast<internal::win9x_critical_section *>(lpCriticalSection);
    if (!cs->check_type())
      return FALSE;

    uintptr_t current_tdbx = internal::win9x_current_tdbx();

    return cs->impl->try_enter(current_tdbx);
  }
} // namespace mingw_thunk
