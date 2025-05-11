#include <libintl.h>

namespace intl
{
  extern "C" char *
  ngettext(const char *msgid, const char *msgid_plural, unsigned long int n)
  {
    return dngettext(nullptr, msgid, msgid_plural, n);
  }
} // namespace intl
