#include "freeaddrinfo.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      ws2_32, 4, VOID, WSAAPI, freeaddrinfo, _In_ PADDRINFOA pAddrInfo)
  {
    __DISPATCH_THUNK_2(freeaddrinfo,
                       const auto pfn = try_get_freeaddrinfo(),
                       pfn,
                       &f::ipv4_freeaddrinfo);

    return dllimport_freeaddrinfo(pAddrInfo);
  }

  namespace f
  {
    // CRT-free version of WspiapiLegacyFreeAddrInfo
    VOID WSAAPI ipv4_freeaddrinfo(_In_ PADDRINFOA pAddrInfo)
    {
      struct addrinfo *p;

      for (p = pAddrInfo; p != NULL; p = pAddrInfo) {
        if (p->ai_canonname)
          c::free(p->ai_canonname);
        if (p->ai_addr)
          c::free(p->ai_addr);
        pAddrInfo = p->ai_next;
        c::free(p);
      }
    }
  } // namespace f
} // namespace mingw_thunk
