#include <thunk/test.h>

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

  SECTION("getnameinfo: invalid address")
  {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    // no flags, invalid address is okay
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

    // NI_NAMEREQD specified, invalid address raises error
    CLEAR_TOUCH_FLAG();
    ret = getnameinfo(reinterpret_cast<sockaddr *>(&addr),
                      sizeof(addr),
                      host,
                      sizeof(host),
                      service,
                      sizeof(service),
                      NI_NAMEREQD);
    REQUIRE_TOUCHED();

    REQUIRE(ret == EAI_NONAME);
  }
}
