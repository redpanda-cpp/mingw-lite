#include "freeaddrinfo.h"

#include <thunk/_common.h>

#include <nocrt/stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      ws2_32, 4, VOID, WSAAPI, freeaddrinfo, _In_ PADDRINFOA pAddrInfo)
  {
    if (auto pfreeaddrinfo = try_get_freeaddrinfo())
      return pfreeaddrinfo(pAddrInfo);

    return impl::ipv4_freeaddrinfo(pAddrInfo);
  }

  namespace impl
  {
    VOID ipv4_freeaddrinfo(_In_ PADDRINFOA pAddrInfo)
    {
      while (pAddrInfo) {
        auto *next = pAddrInfo->ai_next;
        libc::free(pAddrInfo->ai_addr);
        libc::free(pAddrInfo);
        pAddrInfo = next;
      }
    }
  } // namespace impl
} // namespace mingw_thunk
