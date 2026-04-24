#include <libintl.h>

#include <stdlib.h>
#include <string.h>

#include "internal-state.h"
#include "util/owning_ptr.h"

namespace intl
{
  extern "C" char *textdomain(const char *domainname)
  {
    if (!domainname)
      return default_domain.observe();

    size_t len = strlen(domainname);
    char *new_domain = (char *)malloc(len + 1);
    if (!new_domain)
      return nullptr;

    memcpy(new_domain, domainname, len);
    new_domain[len] = 0;

    default_domain = owning_ptr<char[]>(new_domain);
    return new_domain;
  }
} // namespace intl
