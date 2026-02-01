#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/critical_section.h>
#include <thunk/os.h>

#include <synchapi.h>

#include <stdint.h>

namespace mingw_thunk
{
  // Windows 98 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 TryEnterCriticalSection,
                 _Inout_ LPCRITICAL_SECTION lpCriticalSection)
  {

#if THUNK_LEVEL >= NTDDI_WIN98

    if (internal::is_nt())
      return __ms_TryEnterCriticalSection(lpCriticalSection);

#else

    if (internal::is_nt())
      return get_TryEnterCriticalSection()(lpCriticalSection);

#endif

    const auto *cs =
        reinterpret_cast<internal::critical_section_t *>(lpCriticalSection);
    if (!cs->check_type())
      return FALSE;

    return cs->impl()->try_enter();
  }
} // namespace mingw_thunk
