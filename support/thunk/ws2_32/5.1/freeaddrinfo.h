#pragma once

#include <ws2tcpip.h>

namespace mingw_thunk
{
  namespace f
  {
    VOID WSAAPI ipv4_freeaddrinfo(_In_ PADDRINFOA pAddrInfo);
  }
} // namespace mingw_thunk
