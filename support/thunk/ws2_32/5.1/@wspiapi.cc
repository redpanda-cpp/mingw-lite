#include "@wspiapi.h"

#include <thunk/stdlib.h>
#include <thunk/string.h>

// before <windows.h>
#include <winsock2.h>

#include <windows.h>
#include <ws2tcpip.h>

namespace mingw_thunk
{
  namespace i
  {
    int WspiapiClone(WORD wPort, struct addrinfo *ptResult)
    {
      struct addrinfo *p = NULL;
      struct addrinfo *n = NULL;

      for (p = ptResult; p != NULL;) {
        n = WspiapiNewAddrInfo(
            SOCK_DGRAM,
            p->ai_protocol,
            wPort,
            ((struct sockaddr_in *)p->ai_addr)->sin_addr.s_addr);
        if (!n)
          break;
        n->ai_next = p->ai_next;
        p->ai_next = n;
        p = n->ai_next;
      }
      if (p != NULL)
        return EAI_MEMORY;
      return 0;
    }

    int WspiapiLookupNode(const char *pszNodeName,
                          int iSocketType,
                          int iProtocol,
                          WORD wPort,
                          WINBOOL bAI_CANONNAME,
                          struct addrinfo **pptResult)
    {
      int err = 0, cntAlias = 0;
      char name[NI_MAXHOST] = "";
      char alias[NI_MAXHOST] = "";
      char *pname = name, *palias = alias, *tmp = NULL;

      strncpy(pname, pszNodeName, NI_MAXHOST - 1);
      pname[NI_MAXHOST - 1] = 0;
      for (;;) {
        err = WspiapiQueryDNS(
            pszNodeName, iSocketType, iProtocol, wPort, palias, pptResult);
        if (err)
          break;
        if (*pptResult)
          break;
        ++cntAlias;
        if (strlen(palias) == 0 || !strcmp(pname, palias) || cntAlias == 16) {
          err = EAI_FAIL;
          break;
        }
        /* WspiapiSwap(pname, palias, tmp); */ {
          tmp = pname;
          pname = palias;
          palias = tmp;
        }
      }
      if (!err && bAI_CANONNAME) {
        (*pptResult)->ai_canonname = WspiapiStrdup(palias);
        if (!(*pptResult)->ai_canonname)
          err = EAI_MEMORY;
      }
      return err;
    }

    struct addrinfo *WspiapiNewAddrInfo(int iSocketType,
                                        int iProtocol,
                                        WORD wPort,
                                        DWORD dwAddress)
    {
      struct addrinfo *n;
      struct sockaddr_in *pa;

      if ((n = (struct addrinfo *)c::malloc(sizeof(struct addrinfo))) == NULL)
        return NULL;
      if ((pa = (struct sockaddr_in *)c::malloc(sizeof(struct sockaddr_in))) ==
          NULL) {
        c::free(n);
        return NULL;
      }
      pa->sin_family = AF_INET;
      pa->sin_port = wPort;
      pa->sin_addr.s_addr = dwAddress;
      n->ai_family = PF_INET;
      n->ai_socktype = iSocketType;
      n->ai_protocol = iProtocol;
      n->ai_addrlen = sizeof(struct sockaddr_in);
      n->ai_addr = (struct sockaddr *)pa;
      return n;
    }

    bool WspiapiParseV4Address(const char *pszAddress, PDWORD pdwAddress)
    {
      DWORD dwAddress = 0;
      const char *h = NULL;
      int cnt;

      for (cnt = 0, h = pszAddress; *h != 0; h++)
        if (h[0] == '.')
          cnt++;
      if (cnt != 3)
        return FALSE;
      dwAddress = inet_addr(pszAddress);
      if (dwAddress == INADDR_NONE)
        return FALSE;
      *pdwAddress = dwAddress;
      return TRUE;
    }

    int WspiapiQueryDNS(const char *pszNodeName,
                        int iSocketType,
                        int iProtocol,
                        WORD wPort,
                        char pszAlias[NI_MAXHOST],
                        struct addrinfo **pptResult)
    {
      struct addrinfo **paddrinfo = pptResult;
      struct hostent *phost = NULL;
      char **h;

      *paddrinfo = NULL;
      pszAlias[0] = 0;
      phost = gethostbyname(pszNodeName);
      if (phost) {
        if (phost->h_addrtype == AF_INET &&
            phost->h_length == sizeof(struct in_addr)) {
          for (h = phost->h_addr_list; *h != NULL; h++) {
            *paddrinfo = WspiapiNewAddrInfo(
                iSocketType, iProtocol, wPort, ((struct in_addr *)*h)->s_addr);
            if (!*paddrinfo)
              return EAI_MEMORY;
            paddrinfo = &((*paddrinfo)->ai_next);
          }
        }
        strncpy(pszAlias, phost->h_name, NI_MAXHOST - 1);
        pszAlias[NI_MAXHOST - 1] = 0;
        return 0;
      }
      switch (WSAGetLastError()) {
      case WSAHOST_NOT_FOUND:
        break;
      case WSATRY_AGAIN:
        return EAI_AGAIN;
      case WSANO_RECOVERY:
        return EAI_FAIL;
      case WSANO_DATA:
        return EAI_NODATA;
      default:
        break;
      }
      return EAI_NONAME;
    }

    char *WspiapiStrdup(const char *pszString)
    {
      char *rstr;
      size_t szlen;

      if (!pszString)
        return NULL;
      szlen = c::strlen(pszString) + 1;
      rstr = (char *)c::malloc(szlen);
      if (!rstr)
        return NULL;
      c::strcpy(rstr, pszString);
      return rstr;
    }

    uint16_t read_port(const char *src, char **end)
    {
      int h_port = 0;
      while (*src >= '0' && *src <= '9') {
        h_port = h_port * 10 + (*src - '0');
        src++;
      }
      if (end)
        *end = (char *)src;
      return h_port;
    }

    void write_port(char (&dest)[6], uint16_t h_port)
    {
      int digit;
      if (h_port >= 10000)
        digit = 5;
      else if (h_port >= 1000)
        digit = 4;
      else if (h_port >= 100)
        digit = 3;
      else if (h_port >= 10)
        digit = 2;
      else
        digit = 1;

      char *cur = dest;
      switch (digit) {
      case 5:
        *cur++ = '0' + h_port / 10000;
        h_port %= 10000;
        [[fallthrough]];
      case 4:
        *cur++ = '0' + h_port / 1000;
        h_port %= 1000;
        [[fallthrough]];
      case 3:
        *cur++ = '0' + h_port / 100;
        h_port %= 100;
        [[fallthrough]];
      case 2:
        *cur++ = '0' + h_port / 10;
        h_port %= 10;
        [[fallthrough]];
      case 1:
        *cur++ = '0' + h_port;
        *cur = '\0';
      }
    }
  } // namespace i
} // namespace mingw_thunk
