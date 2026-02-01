#include "getaddrinfo.h"

#include <thunk/_common.h>
#include <thunk/addrinfo.h>

#include <nocrt/stdlib.h>

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
    if (auto pfreeaddrinfo = try_get_getaddrinfo())
      return pfreeaddrinfo(pNodeName, pServiceName, pHints, ppResult);

    return impl::ipv4_getaddrinfo(pNodeName, pServiceName, pHints, ppResult);
  }

  namespace impl
  {
    INT ipv4_getaddrinfo(_In_opt_ PCSTR pNodeName,
                         _In_opt_ PCSTR pServiceName,
                         _In_opt_ const ADDRINFOA *pHints,
                         _Out_ PADDRINFOA *ppResult)
    {
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
      if (pServiceName)
        port = libc::atoi(pServiceName);

      struct hostent *host = gethostbyname(pNodeName);
      if (!host)
        return WSAGetLastError();

      int wsa_error = 0;
      addrinfo *result = internal::addrinfo_from_hostent(
          host, flags, socktype, protocol, port, wsa_error);
      if (wsa_error)
        return wsa_error;
      *ppResult = result;
      return 0;
    }
  } // namespace impl
} // namespace mingw_thunk
