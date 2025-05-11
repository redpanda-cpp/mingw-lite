#include <libintl.h>

namespace intl
{
  extern "C" char *gettext(const char *msgid)
  {
    return dgettext(nullptr, msgid);
  }
} // namespace intl
