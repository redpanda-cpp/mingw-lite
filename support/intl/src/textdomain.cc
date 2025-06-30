#include <libintl.h>

#include <nostl/string.h>

#include "internal-state.h"

namespace intl
{
  stl::string default_domain;

  extern "C" char *textdomain(const char *domainname)
  {
    if (!domainname)
      return default_domain.data();

    default_domain = domainname;
    return default_domain.data();
  }
} // namespace intl
