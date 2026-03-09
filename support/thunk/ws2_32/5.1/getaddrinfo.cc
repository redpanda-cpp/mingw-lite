#include "getaddrinfo.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/addrinfo.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(ws2_32,
                 16,
                 INT,
                 WSAAPI,
                 getaddrinfo,
                 _In_opt_ PCSTR pNodeName,
                 _In_opt_ PCSTR pServiceName,
                 _In_opt_ const ADDRINFOA *pHints,
                 _Out_ PADDRINFOA *ppResult)
  {
    __DISPATCH_THUNK_2(getaddrinfo,
                       const auto pfn = try_get_getaddrinfo(),
                       pfn,
                       &f::ipv4_getaddrinfo);

    return dllimport_getaddrinfo(pNodeName, pServiceName, pHints, ppResult);
  }

  namespace f
  {
    INT WSAAPI ipv4_getaddrinfo(_In_opt_ PCSTR pNodeName,
                                _In_opt_ PCSTR pServiceName,
                                _In_opt_ const ADDRINFOA *pHints,
                                _Out_ PADDRINFOA *ppResult)
    {
      if (!pNodeName && !pServiceName)
        return WSAHOST_NOT_FOUND;

      // pServiceName-only is not yet implemented.
      if (!pNodeName)
        return WSAHOST_NOT_FOUND;

      if (pHints &&
          (pHints->ai_family != AF_UNSPEC && pHints->ai_family != AF_INET))
        return WSAEAFNOSUPPORT;

      int flags = 0;
      if (pHints)
        flags = pHints->ai_flags;

      int socktype = SOCK_STREAM;
      if (pHints)
        socktype = pHints->ai_socktype;

      int protocol = IPPROTO_TCP;
      if (pHints)
        protocol = pHints->ai_protocol;
      else if (socktype == SOCK_DGRAM)
        protocol = IPPROTO_UDP;

      int port = 0;
      if (pServiceName) {
        const char *pnum = pServiceName;
        while (*pnum && *pnum >= '0' && *pnum <= '9') {
          port = port * 10 + (*pnum - '0');
          pnum++;
        }
      }

      struct hostent *host = gethostbyname(pNodeName);
      if (!host)
        return WSAGetLastError();

      int wsa_error = 0;
      addrinfo *result = i::addrinfo_from_hostent(
          host, flags, socktype, protocol, port, wsa_error);
      if (wsa_error)
        return wsa_error;
      *ppResult = result;
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
