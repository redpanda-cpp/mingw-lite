#include "freeaddrinfo.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>

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
    VOID WSAAPI ipv4_freeaddrinfo(_In_ PADDRINFOA pAddrInfo)
    {
      while (pAddrInfo) {
        auto *next = pAddrInfo->ai_next;
        HeapFree(GetProcessHeap(), 0, pAddrInfo->ai_addr);
        HeapFree(GetProcessHeap(), 0, pAddrInfo);
        pAddrInfo = next;
      }
    }
  } // namespace f
} // namespace mingw_thunk
