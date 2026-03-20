#pragma once

#include <stdint.h>

#include <winsock2.h>
#include <ws2tcpip.h>

namespace mingw_thunk
{
  namespace i
  {
    int WspiapiClone(WORD wPort, struct addrinfo *ptResult);

    int WspiapiLookupNode(const char *pszNodeName,
                          int iSocketType,
                          int iProtocol,
                          WORD wPort,
                          WINBOOL bAI_CANONNAME,
                          struct addrinfo **pptResult);

    struct addrinfo *WspiapiNewAddrInfo(int iSocketType,
                                        int iProtocol,
                                        WORD wPort,
                                        DWORD dwAddress);

    bool WspiapiParseV4Address(const char *pszAddress, PDWORD pdwAddress);

    int WspiapiQueryDNS(const char *pszNodeName,
                        int iSocketType,
                        int iProtocol,
                        WORD wPort,
                        char pszAlias[NI_MAXHOST],
                        struct addrinfo **pptResult);

    char *WspiapiStrdup(const char *pszString);

    uint16_t read_port(const char *src, char **end);

    void write_port(char (&dest)[6], uint16_t h_port);
  } // namespace i
} // namespace mingw_thunk
