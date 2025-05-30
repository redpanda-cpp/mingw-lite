#include <thunk/_common.h>
#include <thunk/libc/stdlib.h>

#include <ws2tcpip.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, VOID, WSAAPI, freeaddrinfo, _In_ PADDRINFOA pAddrInfo)
  {
    if (auto pfreeaddrinfo = try_get_freeaddrinfo())
      return pfreeaddrinfo(pAddrInfo);

    while (pAddrInfo) {
      auto *next = pAddrInfo->ai_next;
      internal::free(pAddrInfo->ai_addr);
      internal::free(pAddrInfo);
      pAddrInfo = next;
    }
  }
} // namespace mingw_thunk
