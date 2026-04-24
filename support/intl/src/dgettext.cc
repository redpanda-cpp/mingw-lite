#include <libintl.h>

#include <stdint.h>
#include <windows.h>

#include "internal-state.h"

namespace intl
{
  extern "C" char *dgettext(const char *domainname, const char *msgid)
  {
    if (!msgid)
      return nullptr;

    const char *domain_str = domainname ? domainname : default_domain.observe();
    if (!domain_str || !*domain_str)
      return (char *)msgid;

    // special locales disable NLS
    // https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html
    // undocumented: "POSIX" is also special
    // undocumented: ".utf8" is also accepted
    wstring_view special_locale[] = {
        L"C",
        L"C.UTF-8",
        L"C.utf8",
        L"POSIX",
        L"POSIX.UTF-8",
        L"POSIX.utf8",
    };
    const wchar_t *env_lc_all = _wgetenv(L"LC_ALL");
    const wchar_t *env_lc_messages = _wgetenv(L"LC_MESSAGES");
    if (env_lc_all != nullptr && *env_lc_all != L'\0') {
      for (auto locale : special_locale) {
        if (locale == env_lc_all)
          return (char *)msgid;
      }
    } else if (env_lc_messages != nullptr && *env_lc_messages != L'\0') {
      for (auto locale : special_locale) {
        if (locale == env_lc_messages)
          return (char *)msgid;
      }
    }

    string_view domainname_sv = domain_str;
    domain_cache *domain = cache_manager.get_or_load(domainname_sv);
    if (!domain)
      return (char *)msgid;

    return domain->gettext(msgid);
  }
} // namespace intl
