#include "TryEnterCriticalSection.h"

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
    __DISPATCH_THUNK_2(TryEnterCriticalSection,
                       i::is_nt(),
                       &__ms_TryEnterCriticalSection,
                       &f::win9x_TryEnterCriticalSection);
#else
    __DISPATCH_THUNK_2(TryEnterCriticalSection,
                       i::is_nt(),
                       get_TryEnterCriticalSection(),
                       &f::win9x_TryEnterCriticalSection);
#endif

    return dllimport_TryEnterCriticalSection(lpCriticalSection);
  }

  namespace f
  {
    BOOL __stdcall
    win9x_TryEnterCriticalSection(_Inout_ LPCRITICAL_SECTION lpCriticalSection)
    {
      const auto *cs =
          reinterpret_cast<d::critical_section_t *>(lpCriticalSection);
      if (!cs->check_type())
        return FALSE;

      return cs->impl()->try_enter();
    }
  } // namespace f
} // namespace mingw_thunk
