#include "getnameinfo.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/string.h>

#include "@wspiapi.h"

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
    __DISPATCH_THUNK_2(getnameinfo,
                       const auto pfn = try_get_getnameinfo(),
                       pfn,
                       &f::ipv4_getnameinfo);

    return dllimport_getnameinfo(pSockaddr,
                                 SockaddrLength,
                                 pNodeBuffer,
                                 NodeBufferLength,
                                 pServiceBuffer,
                                 ServiceBufferLength,
                                 Flags);
  }

  namespace f
  {
    // CRT-free version of WspiapiLegacyGetNameInfo
    INT WSAAPI ipv4_getnameinfo(_In_ const SOCKADDR *pSockaddr,
                                _In_ socklen_t SockaddrLength,
                                _Out_ PCHAR pNodeBuffer,
                                _In_ DWORD NodeBufferLength,
                                _Out_ PCHAR pServiceBuffer,
                                _In_ DWORD ServiceBufferLength,
                                _In_ INT Flags)
    {
      struct servent *svc;
      WORD port;
      char str[] = "65535";
      char *pstr = str;
      struct hostent *phost;
      struct in_addr l_inaddr;
      char *pnode = NULL, *pc = NULL;

      if (!pSockaddr || SockaddrLength < (int)sizeof(struct sockaddr))
        return EAI_FAIL;
      if (pSockaddr->sa_family != AF_INET)
        return EAI_FAMILY;
      if (SockaddrLength < (int)sizeof(struct sockaddr_in))
        return EAI_FAIL;
      if (!(pNodeBuffer && NodeBufferLength) &&
          !(pServiceBuffer && ServiceBufferLength))
        return EAI_NONAME;
      if ((Flags & NI_NUMERICHOST) != 0 && (Flags & NI_NAMEREQD) != 0)
        return EAI_BADFLAGS;
      if (pServiceBuffer && ServiceBufferLength) {
        port = ((struct sockaddr_in *)pSockaddr)->sin_port;
        if (Flags & NI_NUMERICSERV)
          i::write_port(str, ntohs(port));
        else {
          svc = getservbyport(port, (Flags & NI_DGRAM) ? "udp" : NULL);
          if (svc && svc->s_name)
            pstr = svc->s_name;
          else
            i::write_port(str, ntohs(port));
        }
        if (ServiceBufferLength > c::strlen(pstr))
          c::strcpy(pServiceBuffer, pstr);
        else
          return EAI_FAIL;
      }
      if (pNodeBuffer && NodeBufferLength) {
        l_inaddr = ((struct sockaddr_in *)pSockaddr)->sin_addr;
        if (Flags & NI_NUMERICHOST)
          pnode = inet_ntoa(l_inaddr);
        else {
          phost =
              gethostbyaddr((char *)&l_inaddr, sizeof(struct in_addr), AF_INET);
          if (phost && phost->h_name) {
            pnode = phost->h_name;
            if ((Flags & NI_NOFQDN) != 0 && ((pc = strchr(pnode, '.')) != NULL))
              *pc = 0;
          } else {
            if ((Flags & NI_NAMEREQD) != 0) {
              switch (WSAGetLastError()) {
              case WSAHOST_NOT_FOUND:
                return EAI_NONAME;
              case WSATRY_AGAIN:
                return EAI_AGAIN;
              case WSANO_RECOVERY:
                return EAI_FAIL;
              default:
                return EAI_NONAME;
              }
            } else
              pnode = inet_ntoa(l_inaddr);
          }
        }
        if (NodeBufferLength > c::strlen(pnode))
          c::strcpy(pNodeBuffer, pnode);
        else
          return EAI_FAIL;
      }
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
