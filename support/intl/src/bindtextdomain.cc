#include <libintl.h>

namespace intl
{
  extern "C" char *bindtextdomain(const char *domainname [[maybe_unused]],
                                  const char *dirname [[maybe_unused]])
  {
    return nullptr;
  }
} // namespace intl
