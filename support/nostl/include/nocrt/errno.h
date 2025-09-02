#pragma once

#include <processthreadsapi.h>
#include <winternl.h>

#include "__config.h"

namespace NS_NOCRT
{
  inline DWORD __errno_tls_index()
  {
    static DWORD tls_index = TlsAlloc();
    return tls_index;
  }

  inline int *__errno_location()
  {
    DWORD tls_index = __errno_tls_index();
    PVOID *pslot;
    if (tls_index < TLS_MINIMUM_AVAILABLE) {
      pslot = &NtCurrentTeb()->TlsSlots[tls_index];
    } else {
      PVOID *expansion_slots =
          static_cast<PVOID *>(NtCurrentTeb()->TlsExpansionSlots);
      pslot = &expansion_slots[tls_index - TLS_MINIMUM_AVAILABLE];
    }
    return reinterpret_cast<int *>(pslot);
  }

} // namespace NS_NOCRT

#define NOCRT_errno (*NS_NOCRT::__errno_location())

#ifdef ENOMEM
static_assert(ENOMEM == 12, "ENOMEM != 12");
#else
#define ENOMEM 12
#endif

#ifdef EINVAL
static_assert(EINVAL == 22, "EINVAL != 22");
#else
#define EINVAL 22
#endif
