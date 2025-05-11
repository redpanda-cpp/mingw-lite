#include <libintl.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "internal-state.h"

namespace intl
{
  extern "C" char *textdomain(const char *domainname)
  {
    size_t len = strlen(domainname);
    char *p = (char *)malloc(len + 1);
    if (!p) {
      _set_errno(ENOMEM);
      return nullptr;
    }

    memcpy(p, domainname, len + 1);
    default_domain = {p, &free};
    return p;
  }
} // namespace intl
