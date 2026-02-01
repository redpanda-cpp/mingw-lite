#include "getaddrinfo.h"
#include "freeaddrinfo.h"

#include <catch_amalgamated.hpp>

TEST_CASE("getaddrinfo and freeaddrinfo")
{
  SECTION("initialize Winsock")
  {
    WSADATA wsaData;
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    REQUIRE(ret == 0);
  }

  SECTION("getaddrinfo: numeric service")
  {
    struct addrinfo *res = nullptr;

    int ret =
        mingw_thunk::impl::ipv4_getaddrinfo("localhost", "8080", nullptr, &res);

    REQUIRE(ret == 0);
    REQUIRE(res != nullptr);

    REQUIRE(res->ai_family == AF_INET);
    REQUIRE(res->ai_addrlen == sizeof(struct sockaddr_in));

    auto *addr = reinterpret_cast<sockaddr_in *>(res->ai_addr);
    REQUIRE(addr->sin_family == AF_INET);
    REQUIRE(addr->sin_port == htons(8080));
    REQUIRE(addr->sin_addr.s_addr == htonl(INADDR_LOOPBACK));

    mingw_thunk::impl::ipv4_freeaddrinfo(res);
  }

  SECTION("getaddrinfo: invalid node")
  {
    struct addrinfo *res = nullptr;

    int ret = mingw_thunk::impl::ipv4_getaddrinfo(
        "invalid-node.invalid-domain", nullptr, nullptr, &res);

    REQUIRE(ret == WSAHOST_NOT_FOUND);
  }
}
