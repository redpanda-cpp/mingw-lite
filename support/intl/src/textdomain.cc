#include <libintl.h>

#include <errno.h>
#include <new>
#include <stdlib.h>
#include <string.h>

#include "internal-state.h"

namespace intl
{
  extern "C" char *textdomain(const char *domainname)
  {
    if (!domainname)
      return default_domain.data();

    try {
      default_domain = domainname;
      return default_domain.data();
    } catch (const std::bad_alloc &) {
      _set_errno(ENOMEM);
      return nullptr;
    }
  }
} // namespace intl
