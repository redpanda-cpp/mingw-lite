#pragma once

#include <ws2tcpip.h>

namespace mingw_thunk::impl
{
  INT ipv4_getnameinfo(_In_ const SOCKADDR *pSockaddr,
                       _In_ socklen_t SockaddrLength,
                       _Out_ PCHAR pNodeBuffer,
                       _In_ DWORD NodeBufferLength,
                       _Out_ PCHAR pServiceBuffer,
                       _In_ DWORD ServiceBufferLength,
                       _In_ INT Flags);
}
