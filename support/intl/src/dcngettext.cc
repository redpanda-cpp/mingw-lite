#include <libintl.h>

namespace intl
{
  extern "C" char *dcngettext(const char *domainname,
                              const char *msgid,
                              const char *msgid_plural,
                              unsigned long int n,
                              int category [[maybe_unused]])
  {
    return dngettext(domainname, msgid, msgid_plural, n);
  }
} // namespace intl
