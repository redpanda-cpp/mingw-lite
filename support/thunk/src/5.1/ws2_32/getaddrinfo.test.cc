#include <thunk/test.h>

#include <ws2tcpip.h>

TEST_CASE("getaddrinfo, getnameinfo and freeaddrinfo")
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

    CLEAR_TOUCH_FLAG();
    int ret = getaddrinfo("localhost", "8080", nullptr, &res);
    REQUIRE_TOUCHED();

    REQUIRE(ret == 0);
    REQUIRE(res != nullptr);

    REQUIRE(res->ai_family == AF_INET);
    REQUIRE(res->ai_addrlen == sizeof(struct sockaddr_in));

    auto *addr = reinterpret_cast<sockaddr_in *>(res->ai_addr);
    REQUIRE(addr->sin_family == AF_INET);
    REQUIRE(addr->sin_port == htons(8080));
    REQUIRE(addr->sin_addr.s_addr == htonl(INADDR_LOOPBACK));

    CLEAR_TOUCH_FLAG();
    freeaddrinfo(res);
    REQUIRE_TOUCHED();
  }

  SECTION("getaddrinfo: invalid node")
  {
    struct addrinfo *res = nullptr;

    CLEAR_TOUCH_FLAG();
    int ret =
        getaddrinfo("invalid-node.invalid-domain", nullptr, nullptr, &res);
    REQUIRE_TOUCHED();

    REQUIRE(ret == WSAHOST_NOT_FOUND);
  }

  SECTION("getnameinfo")
  {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    CLEAR_TOUCH_FLAG();
    int ret = getnameinfo(reinterpret_cast<sockaddr *>(&addr),
                          sizeof(addr),
                          host,
                          sizeof(host),
                          service,
                          sizeof(service),
                          0);
    REQUIRE_TOUCHED();

    REQUIRE(ret == 0);
  }
}
