#pragma once

#include <ws2tcpip.h>

namespace mingw_thunk::impl
{
  INT ipv4_getaddrinfo(_In_opt_ PCSTR pNodeName,
                       _In_opt_ PCSTR pServiceName,
                       _In_opt_ const ADDRINFOA *pHints,
                       _Out_ PADDRINFOA *ppResult);
}
