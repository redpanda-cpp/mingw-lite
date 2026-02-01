#pragma once

#include <ws2tcpip.h>

namespace mingw_thunk::impl
{
  VOID ipv4_freeaddrinfo(_In_ PADDRINFOA pAddrInfo);
}
