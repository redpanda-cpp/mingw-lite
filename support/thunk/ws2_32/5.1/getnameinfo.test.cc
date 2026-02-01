#include "getnameinfo.h"

#include <catch_amalgamated.hpp>

#include <ws2tcpip.h>

TEST_CASE("getnameinfo")
{
  SECTION("initialize Winsock")
  {
    WSADATA wsaData;
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    REQUIRE(ret == 0);
  }

  SECTION("getnameinfo: normal case")
  {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int ret =
        mingw_thunk::impl::ipv4_getnameinfo(reinterpret_cast<sockaddr *>(&addr),
                                            sizeof(addr),
                                            host,
                                            sizeof(host),
                                            service,
                                            sizeof(service),
                                            0);
    REQUIRE(ret == 0);
  }

  SECTION("getnameinfo: invalid address")
  {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    // no flags, invalid address is okay
    int ret =
        mingw_thunk::impl::ipv4_getnameinfo(reinterpret_cast<sockaddr *>(&addr),
                                            sizeof(addr),
                                            host,
                                            sizeof(host),
                                            service,
                                            sizeof(service),
                                            0);
    REQUIRE(ret == 0);

    // NI_NAMEREQD specified, invalid address raises error
    ret =
        mingw_thunk::impl::ipv4_getnameinfo(reinterpret_cast<sockaddr *>(&addr),
                                            sizeof(addr),
                                            host,
                                            sizeof(host),
                                            service,
                                            sizeof(service),
                                            NI_NAMEREQD);
    REQUIRE(ret == EAI_NONAME);
  }
}
