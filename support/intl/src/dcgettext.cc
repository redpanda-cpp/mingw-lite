#include <libintl.h>

namespace intl
{
  extern "C" char *dcgettext(const char *domainname,
                             const char *msgid,
                             int category [[maybe_unused]])
  {
    return dgettext(domainname, msgid);
  }
} // namespace intl
