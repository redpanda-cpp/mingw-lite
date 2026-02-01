#include "getnameinfo.h"

#include <thunk/_common.h>
#include <thunk/addrinfo.h>

#include <nocrt/string.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(ws2_32,
                 28,
                 INT,
                 WSAAPI,
                 getnameinfo,
                 _In_ const SOCKADDR *pSockaddr,
                 _In_ socklen_t SockaddrLength,
                 _Out_ PCHAR pNodeBuffer,
                 _In_ DWORD NodeBufferLength,
                 _Out_ PCHAR pServiceBuffer,
                 _In_ DWORD ServiceBufferLength,
                 _In_ INT Flags)
  {
    if (const auto pfn = try_get_getnameinfo())
      return pfn(pSockaddr,
                 SockaddrLength,
                 pNodeBuffer,
                 NodeBufferLength,
                 pServiceBuffer,
                 ServiceBufferLength,
                 Flags);

    return impl::ipv4_getnameinfo(pSockaddr,
                                  SockaddrLength,
                                  pNodeBuffer,
                                  NodeBufferLength,
                                  pServiceBuffer,
                                  ServiceBufferLength,
                                  Flags);
  }

  namespace impl
  {
    INT ipv4_getnameinfo(_In_ const SOCKADDR *pSockaddr,
                         _In_ socklen_t SockaddrLength,
                         _Out_ PCHAR pNodeBuffer,
                         _In_ DWORD NodeBufferLength,
                         _Out_ PCHAR pServiceBuffer,
                         _In_ DWORD ServiceBufferLength,
                         _In_ INT Flags)
    {
      if (pServiceBuffer && ServiceBufferLength > 0) {
        int port = ntohs(((sockaddr_in *)pSockaddr)->sin_port);
        if ((port >= 10000 && ServiceBufferLength < 6) ||
            (port >= 1000 && ServiceBufferLength < 5) ||
            (port >= 100 && ServiceBufferLength < 4) ||
            (port >= 10 && ServiceBufferLength < 3) || ServiceBufferLength < 2)
          return WSAEFAULT;
        char *service = pServiceBuffer;
        if (port >= 10000)
          *service++ = '0' + port / 10000;
        if (port >= 1000)
          *service++ = '0' + port / 1000 % 10;
        if (port >= 100)
          *service++ = '0' + port / 100 % 10;
        if (port >= 10)
          *service++ = '0' + port / 10 % 10;
        *service++ = '0' + port % 10;
        *service++ = '\0';
      }

      if (SockaddrLength == sizeof(sockaddr_in)) {
        const sockaddr_in *addr = (const sockaddr_in *)pSockaddr;
        hostent *host = gethostbyaddr(
            (const char *)&addr->sin_addr, sizeof(in_addr), AF_INET);
        if (!host && (Flags & NI_NAMEREQD)) {
          WSASetLastError(EAI_NONAME);
          return EAI_NONAME;
        }

        if (host && pNodeBuffer && NodeBufferLength > 0) {
          libc::stpncpy(pNodeBuffer, host->h_name, NodeBufferLength);
          pNodeBuffer[NodeBufferLength - 1] = '\0';
        }
        return 0;
      } else if (SockaddrLength == sizeof(sockaddr_in6)) {
        const sockaddr_in6 *addr = (const sockaddr_in6 *)pSockaddr;
        hostent *host = gethostbyaddr(
            (const char *)&addr->sin6_addr, sizeof(in6_addr), AF_INET6);
        if (!host && (Flags & NI_NAMEREQD)) {
          WSASetLastError(EAI_NONAME);
          return EAI_NONAME;
        }

        if (host && pNodeBuffer && NodeBufferLength > 0) {
          libc::stpncpy(pNodeBuffer, host->h_name, NodeBufferLength);
          pNodeBuffer[NodeBufferLength - 1] = '\0';
        }
        return 0;
      } else {
        return WSAEFAULT;
      }
    }
  } // namespace impl
} // namespace mingw_thunk
