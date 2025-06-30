#include <libintl.h>

#include <nostl/algorithm.h>
#include <nostl/map.h>
#include <nostl/string.h>
#include <nostl/string_view.h>
#include <nostl/utility.h>

#include <stdint.h>
#include <windows.h>

#include "file.h"
#include "internal-state.h"
#include "raw_data.h"

namespace intl
{
  using stl::map;
  using stl::string;
  using stl::string_view;
  using stl::wstring;
  using stl::wstring_view;

  // https://www.gnu.org/software/gettext/manual/html_node/MO-Files.html
  struct mo_header
  {
    uint32_t magic_number;
    uint32_t file_format_revision;
    uint32_t N;
    uint32_t O;
    uint32_t T;
    uint32_t S;
    uint32_t H;
  };

  struct mo_string
  {
    uint32_t length;
    uint32_t offset;
  };

  struct mo_file
  {
    mo_header *header;
    size_t size;
  };

  const bool system_is_utf8 = GetACP() == CP_UTF8;

  struct domain_cache
  {
  public:
    raw_data data;
    map<string_view, char *> cached_translations_utf8;
    map<string_view, string> cached_translations_local;

  public:
    char *gettext(string_view msgid)
    {
      if (!data)
        return (char *)msgid.data();

      // is cached?
      if (system_is_utf8) {
        auto it = cached_translations_utf8.find(msgid);
        if (it != cached_translations_utf8.end())
          return it->second ? it->second : (char *)msgid.data();
      } else {
        auto it = cached_translations_local.find(msgid);
        if (it != cached_translations_local.end())
          return it->second.data();
      }

      // try load from mo file
      mo_header *header = data.as<mo_header>();
      mo_string *o_table = (data + header->O).as<mo_string>();
      mo_string *t_table = (data + header->T).as<mo_string>();

      auto compare = [this](mo_string elem, string_view msgid) {
        string_view s = {data + elem.offset, elem.length};
        return s < msgid;
      };

      auto it = stl::lower_bound(o_table, o_table + header->N, msgid, compare);
      if (it == o_table + header->N)
        return (char *)msgid.data();

      size_t idx = it - o_table;
      string_view o_sv = {data + o_table[idx].offset, o_table[idx].length};
      if (o_sv != msgid)
        return (char *)msgid.data();

      // found
      if (system_is_utf8) {
        char *t = data + t_table[idx].offset;
        cached_translations_utf8[o_sv] = t;
        return t;
      } else {
        char *t = data + t_table[idx].offset;
        size_t length = t_table[idx].length;

        int wlen = MultiByteToWideChar(CP_UTF8, 0, t, length, nullptr, 0);
        wstring wbuf(wlen, 0);
        MultiByteToWideChar(CP_UTF8, 0, t, length, wbuf.data(), wlen);

        BOOL fail;
        int alen = WideCharToMultiByte(
            CP_ACP, 0, wbuf.data(), wlen, nullptr, 0, nullptr, &fail);
        if (fail)
          return (char *)msgid.data();
        string abuf(alen, 0);
        WideCharToMultiByte(
            CP_ACP, 0, wbuf.data(), wlen, abuf.data(), alen, nullptr, nullptr);
        cached_translations_local[o_sv] = stl::move(abuf);
        return cached_translations_local[o_sv].data();
      }
    }

  public:
    static domain_cache from_file(file &&fp)
    {
      domain_cache result;

      fp.seek(0, SEEK_END);
      size_t size = fp.tell();
      fp.seek(0, SEEK_SET);

      if (sizeof(mo_header) > size)
        return result;

      raw_data data = malloc(size);
      if (!data)
        RaiseException(STATUS_NO_MEMORY, 0, 0, nullptr);
      if (fp.read(data, 1, size) != size)
        return result;

      mo_header *header = data.as<mo_header>();
      if (header->magic_number != 0x950412de)
        return result;
      if (header->O + header->N * sizeof(mo_string) > size)
        return result;
      if (header->T + header->N * sizeof(mo_string) > size)
        return result;

      auto o_table = (data + header->O).as<mo_string>();
      auto t_table = (data + header->T).as<mo_string>();
      for (uint32_t i = 0; i < header->N; i++) {
        if (o_table[i].offset + o_table[i].length > size)
          return result;
        if (t_table[i].offset + t_table[i].length > size)
          return result;
      }

      // okay, save it
      result.data = stl::move(data);
      return result;
    }
  };

  struct cache_manager_t
  {
  public:
    map<string, domain_cache> cached_domains;

  public:
    domain_cache &operator[](const string &domain)
    {
      auto it = cached_domains.find(domain);
      if (it != cached_domains.end())
        return it->second;

      wstring prefix = resolve_prefix();
      if (prefix.empty())
        return cached_domains[domain];

      // search order:
      // 1. "LANGUAGE"
      // 2. "LC_ALL"
      // 3. "LC_xxx"
      // 4. "LANG"
      // https://www.gnu.org/software/gettext/manual/html_node/Locale-Environment-Variables.html
      // undocumented: GNU gettext will try GetThreadLocale()
      // undocumented: set console to UTF-8 breaks GetThreadLocale()
      // extension: use GetUserPreferredUILanguages() instead
      const wchar_t *env_language = _wgetenv(L"LANGUAGE");
      const wchar_t *env_lc_all = _wgetenv(L"LC_ALL");
      const wchar_t *env_lc_messages = _wgetenv(L"LC_MESSAGES");
      const wchar_t *env_lang = _wgetenv(L"LANG");

      file fp;
      if (env_language != nullptr && *env_language != L'\0')
        fp = find_locale_file_by_env_language(prefix, env_language, domain);
      else if (env_lc_all != nullptr && *env_lc_all != L'\0')
        fp = find_locale_file(prefix, env_lc_all, domain);
      else if (env_lc_messages != nullptr && *env_lc_messages != L'\0')
        fp = find_locale_file(prefix, env_lc_messages, domain);
      else if (env_lang != nullptr && *env_lang != L'\0')
        fp = find_locale_file(prefix, env_lang, domain);
      else
        fp = find_locale_file_from_win32(prefix, domain);
      if (!fp)
        return cached_domains[domain];

      auto entry = domain_cache::from_file(stl::move(fp));
      cached_domains[domain] = stl::move(entry);
      return cached_domains[domain];
    }

  private:
    wstring resolve_prefix()
    {
      wstring path(MAX_PATH, 0);
      DWORD exe_len = GetModuleFileNameW(nullptr, &path[0], MAX_PATH);
      if (exe_len >= MAX_PATH)
        return {};

      // \lib\gcc\<target>\<version>\cc1.exe
      wstring_view target = LR"(\lib\gcc\)";
      size_t pos = path.rfind(target);
      if (pos == wstring::npos) {
        target = LR"(\bin\)";
        pos = path.rfind(target);
      }
      if (pos == wstring::npos)
        return {};

      path.resize(pos);
      path += LR"(\share\locale\)";
      return path;
    }

    file find_locale_file_by_env_language(wstring_view prefix,
                                          wstring_view env_language,
                                          string_view domain)
    {
      // LANGUAGE=zh_CN:en_US
      size_t b = 0;
      while (true) {
        size_t e = env_language.find(L':', b);
        wstring_view lang;
        if (e == wstring_view::npos)
          lang = env_language.substr(b);
        else
          lang = env_language.substr(b, e - b);
        file fp = find_locale_file(prefix, lang, domain);
        if (fp)
          return fp;
        if (e == wstring_view::npos)
          break;
        else
          b = e + 1;
      }
      return {};
    }

    file find_locale_file_by_win32_language_name(wstring_view prefix,
                                                 wstring_view language_name,
                                                 string_view domain)
    {
      // https://learn.microsoft.com/en-us/windows/win32/intl/language-names

      // remove supplemental: en-US-x-fabricam
      wstring_view infix = L"-x-";
      auto pos = language_name.find(infix);
      if (pos != wstring_view::npos)
        language_name = language_name.substr(0, pos);

      auto pos1 = language_name.find(L'-');
      if (pos1 == wstring_view::npos) {
        // neutral
        return find_locale_file(prefix, language_name, domain);
      }

      auto pos2 = language_name.find(L'-', pos1 + 1);
      if (pos2 == wstring_view::npos) {
        // ll-CC
        wstring gnu_name{language_name.data(), language_name.size()};
        gnu_name[pos1] = L'_';
        return find_locale_file(prefix, gnu_name, domain);
      } else {
        // ll-Script-CC
        // Win32 script name differs from GNU ones, ignore it.
        wstring gnu_name{language_name.data(), pos1};
        gnu_name.push_back(L'_');
        wstring_view region_code = language_name.substr(pos2 + 1);
        gnu_name.append(region_code);
        return find_locale_file(prefix, gnu_name, domain);
      }
    }

    file find_locale_file_from_win32(wstring_view prefix, string_view domain)
    {
      static auto pfn = (decltype(&GetUserPreferredUILanguages))GetProcAddress(
          GetModuleHandleW(L"kernel32.dll"), "GetUserPreferredUILanguages");
      if (!pfn)
        return {};

      wchar_t lang_buf[MAX_PATH];
      ULONG lang_buf_len = MAX_PATH;
      ULONG n_lang = 0;
      if (!pfn(MUI_LANGUAGE_NAME, &n_lang, lang_buf, &lang_buf_len))
        return {};

      wchar_t *p = lang_buf;
      while (*p) {
        wstring_view language_name = p;
        file fp = find_locale_file_by_win32_language_name(
            prefix, language_name, domain);
        if (fp)
          return fp;
        p += language_name.size() + 1;
      }

      return {};
    }

    file find_locale_file(wstring_view prefix,
                          wstring_view locale,
                          string_view domain)
    {
      // try full locale
      wstring path = compose_locale_file_path(prefix, locale, domain);
      file fp(path.c_str(), L"rb");
      if (fp)
        return fp;

      // try without '.encoding' and '@variant'
      size_t pos = locale.find_first_of(L".@");
      if (pos != string::npos) {
        path = compose_locale_file_path(prefix, locale.substr(0, pos), domain);
        fp = file(path.c_str(), L"rb");
        if (fp)
          return fp;
      }

      // try without '_CC'
      pos = locale.find(L'_');
      if (pos != string::npos) {
        path = compose_locale_file_path(prefix, locale.substr(0, pos), domain);
        fp = file(path.c_str(), L"rb");
        if (fp)
          return fp;
      }

      return {};
    }

    wstring compose_locale_file_path(wstring_view prefix,
                                     wstring_view locale,
                                     string_view domain)
    {
      constexpr wstring_view infix{LR"(\LC_MESSAGES\)"};
      constexpr wstring_view suffix{L".mo"};

      wstring result{prefix.data(), prefix.size()};
      result.append(locale.data(), locale.size());
      result.append(infix.data(), infix.size());

      {
        size_t old_size = result.size();
        result.append(domain.size(), 0);
        for (size_t i = 0; i < domain.size(); ++i)
          result[old_size + i] = domain[i];
      }

      result.append(suffix.data(), suffix.size());
      return result;
    }
  };

  cache_manager_t cache_manager;

  extern "C" char *dgettext(const char *domainname, const char *msgid)
  {
    if (!msgid)
      return nullptr;

    const string &domainname_str = domainname ? domainname : default_domain;
    if (domainname_str.empty())
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

    auto &domain = cache_manager[domainname_str];

    return domain.gettext(msgid);
  }
} // namespace intl
