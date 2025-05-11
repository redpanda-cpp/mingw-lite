#include <libintl.h>

namespace intl
{
  extern "C" char *dngettext(const char *domainname,
                             const char *msgid,
                             const char *msgid_plural,
                             unsigned long int n)
  {
    return dgettext(domainname, n == 1 ? msgid : msgid_plural);
  }
} // namespace intl
