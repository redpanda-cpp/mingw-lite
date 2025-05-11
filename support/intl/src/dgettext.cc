#include <libintl.h>

#include <algorithm>
#include <string_view>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <windows.h>

#include "internal-state.h"

using std::string_view;
using std::wstring_view;

namespace intl
{
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

  size_t locate_prefix(const wchar_t *exe_path, size_t exe_len)
  {
    wstring_view search_infix[]{
        LR"(\lib\gcc\)",
        LR"(\bin\)",
    };
    for (auto infix : search_infix) {
      for (ssize_t i = exe_len - infix.length(); i >= 0; i--) {
        if (infix == wstring_view{exe_path + i, infix.length()}) {
          return i;
        }
      }
    }
    return size_t(-1);
  }

  bool compose_locale_file_path(wchar_t *path,
                                size_t locale_dir_len,
                                wstring_view locale,
                                string_view domain)
  {
    wstring_view infix = LR"(\LC_MESSAGES\)";
    wstring_view suffix = L".mo";
    if (locale_dir_len + infix.length() + domain.length() + suffix.length() +
            1 >
        PATH_MAX)
      return false;

    wchar_t *p = path + locale_dir_len;
    memcpy(p, locale.data(), locale.length() * sizeof(wchar_t));
    p += locale.length();
    memcpy(p, infix.data(), infix.length() * sizeof(wchar_t));
    p += infix.length();
    for (size_t i = 0; i < domain.length(); ++i)
      *p++ = domain[i];
    memcpy(p, suffix.data(), suffix.length() * sizeof(wchar_t));
    p += suffix.length();
    *p = 0;
    return true;
  }

  FILE *find_locale_file(wchar_t *path,
                         size_t locale_dir_len,
                         wstring_view locale,
                         string_view domain)
  {
    // try full locale
    if (compose_locale_file_path(path, locale_dir_len, locale, domain)) {
      FILE *fp = _wfopen(path, L"rb");
      if (fp)
        return fp;
    }

    // try without '.encoding' and '@variant'
    for (int i = 0; i < locale.length(); i++) {
      if (locale[i] == L'.' || locale[i] == L'@') {
        if (compose_locale_file_path(
                path, locale_dir_len, locale.substr(0, i), domain)) {
          FILE *fp = _wfopen(path, L"rb");
          if (fp)
            return fp;
        }
        break;
      }
    }

    // try without '_CC'
    for (int i = 0; i < locale.length(); i++) {
      if (locale[i] == L'_') {
        if (compose_locale_file_path(
                path, locale_dir_len, locale.substr(0, i), domain)) {
          FILE *fp = _wfopen(path, L"rb");
          if (fp)
            return fp;
        }
        break;
      }
    }

    return nullptr;
  }

  FILE *find_locale_file_by_env_language(wchar_t *path,
                                         size_t locale_dir_len,
                                         wstring_view env_language,
                                         string_view domain)
  {
    // LANGUAGE=zh_CN:en_US
    size_t b = 0;
    while (b < env_language.size()) {
      size_t e = b;
      while (env_language[e] != L'\0' && env_language[e] != L':')
        ++e;
      FILE *fp = find_locale_file(
          path, locale_dir_len, env_language.substr(b, e - b), domain);
      if (fp)
        return fp;
      b = e + 1;
    }
    return nullptr;
  }

  FILE *find_locale_file_by_win32_language_name(wchar_t *path,
                                                size_t locale_dir_len,
                                                wstring_view language_name,
                                                string_view domain)
  {
    // https://learn.microsoft.com/en-us/windows/win32/intl/language-names

    // remove supplemental: en-US-x-fabricam
    wstring_view infix = L"-x-";
    if (auto pos = language_name.find(infix); pos != wstring_view::npos)
      language_name = language_name.substr(0, pos);

    auto pos1 = language_name.find(L'-');
    if (pos1 == wstring_view::npos) {
      // neutral
      return find_locale_file(path, locale_dir_len, language_name, domain);
    }

    wchar_t gnu_name[MAX_PATH];
    auto pos2 = language_name.find(L'-', pos1 + 1);
    if (pos2 == wstring_view::npos) {
      // ll-CC
      memcpy(gnu_name,
             language_name.data(),
             language_name.size() * sizeof(wchar_t));
      gnu_name[pos1] = L'_';
      gnu_name[language_name.size()] = L'\0';
      return find_locale_file(path, locale_dir_len, gnu_name, domain);
    } else {
      // ll-Script-CC
      // Win32 script name differs from GNU ones, ignore it.
      wchar_t *p = gnu_name;
      size_t country_len = language_name.size() - pos2;
      memcpy(p, language_name.data(), pos1 * sizeof(wchar_t));
      p += pos1;
      *p++ = L'_';
      memcpy(p, language_name.data() + pos2, country_len * sizeof(wchar_t));
      p += country_len;
      *p++ = L'\0';
      return find_locale_file(path, locale_dir_len, gnu_name, domain);
    }
  }

  FILE *find_locale_file_from_win32(wchar_t *path,
                                    size_t locale_dir_len,
                                    string_view domain)
  {
    static auto pfn = (decltype(&GetUserPreferredUILanguages))GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "GetUserPreferredUILanguages");
    if (!pfn)
      return nullptr;

    wchar_t lang_buf[MAX_PATH];
    ULONG lang_buf_len = MAX_PATH;
    ULONG n_lang = 0;
    if (!pfn(MUI_LANGUAGE_NAME, &n_lang, lang_buf, &lang_buf_len))
      return nullptr;

    wchar_t *p = lang_buf;
    while (*p) {
      wstring_view language_name = p;
      FILE *fp = find_locale_file_by_win32_language_name(
          path, locale_dir_len, language_name, domain);
      if (fp)
        return fp;
      p += language_name.length() + 1;
    }

    return nullptr;
  }

  mo_file get_mo_data(string_view domain)
  {
    if (!cached_domain.get() || domain != cached_domain.get()) {
      // duplicate domain (for cached_domain)
      char *p_domain = (char *)malloc(domain.length() + 1);
      if (!p_domain)
        return {nullptr, 0};
      c_unique_ptr<char[]> new_domain = {p_domain, &free};
      memcpy(p_domain, domain.data(), domain.length() + 1);

      wchar_t path[MAX_PATH] = {0};
      DWORD exe_len = GetModuleFileNameW(nullptr, path, MAX_PATH);
      if (exe_len >= MAX_PATH)
        return {nullptr, 0};

      size_t prefix_len = locate_prefix(path, exe_len);
      wstring_view infix = LR"(\share\locale\)";
      if (prefix_len + infix.length() >= MAX_PATH)
        return {nullptr, 0};
      memcpy(path + prefix_len, infix.data(), infix.length() * sizeof(wchar_t));
      size_t locale_dir_len = prefix_len + infix.length();

      // search order:
      // 1. "LANGUAGE"
      // 2. "LC_ALL"
      // 3. "LC_xxx"
      // 4. "LANG"
      // https://www.gnu.org/software/gettext/manual/html_node/Locale-Environment-Variables.html
      // undocumented: GNU gettext will try GetThreadLocale()
      // undocumented: set console to UTF-8 breaks GetThreadLocale()
      // extension: use GetUserPreferredUILanguages() instead
      FILE *fp;
      const wchar_t *env_language = _wgetenv(L"LANGUAGE");
      const wchar_t *env_lc_all = _wgetenv(L"LC_ALL");
      const wchar_t *env_lc_messages = _wgetenv(L"LC_MESSAGES");
      const wchar_t *env_lang = _wgetenv(L"LANG");
      if (env_language != nullptr && *env_language != L'\0')
        fp = find_locale_file_by_env_language(
            path, locale_dir_len, env_language, domain);
      else if (env_lc_all != nullptr && *env_lc_all != L'\0')
        fp = find_locale_file(path, locale_dir_len, env_lc_all, domain);
      else if (env_lc_messages != nullptr && *env_lc_messages != L'\0')
        fp = find_locale_file(path, locale_dir_len, env_lc_messages, domain);
      else if (env_lang != nullptr && *env_lang != L'\0')
        fp = find_locale_file(path, locale_dir_len, env_lang, domain);
      else
        fp = find_locale_file_from_win32(path, locale_dir_len, domain);
      if (!fp)
        return {nullptr, 0};

      std::unique_ptr<FILE, decltype(&fclose)> fp_guard(fp, &fclose);

      fseek(fp, 0, SEEK_END);
      size_t new_size = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      if (sizeof(mo_header) > new_size)
        return {nullptr, 0};

      char *p_data = (char *)malloc(new_size);
      if (!p_data)
        return {nullptr, 0};
      c_unique_ptr<char[]> new_data = c_unique_ptr<char[]>(p_data, &free);
      if (fread(p_data, 1, new_size, fp) != new_size)
        return {nullptr, 0};

      mo_header *header = (mo_header *)p_data;
      if (header->magic_number != 0x950412de)
        return {nullptr, 0};
      if (header->O + header->N * sizeof(mo_string) > new_size)
        return {nullptr, 0};
      if (header->T + header->N * sizeof(mo_string) > new_size)
        return {nullptr, 0};

      auto o_table = (mo_string *)(p_data + header->O);
      auto t_table = (mo_string *)(p_data + header->T);
      for (uint32_t i = 0; i < header->N; i++) {
        if (o_table[i].offset + o_table[i].length > new_size)
          return {nullptr, 0};
        if (t_table[i].offset + t_table[i].length > new_size)
          return {nullptr, 0};
      }

      // okay, save it
      cached_domain = std::move(new_domain);
      cached_data = std::move(new_data);
      cached_size = new_size;
    }

    return {(mo_header *)cached_data.get(), cached_size};
  }

  extern "C" char *dgettext(const char *domainname, const char *msgid)
  {
    if (!msgid)
      return nullptr;
    if (!domainname)
      domainname = default_domain.get();
    if (!domainname)
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
    if (const wchar_t *env_lc_all = _wgetenv(L"LC_ALL");
        env_lc_all != nullptr && *env_lc_all != L'\0') {
      for (auto locale : special_locale) {
        if (env_lc_all == locale)
          return (char *)msgid;
      }
    } else if (const wchar_t *env_lc_messages = _wgetenv(L"LC_MESSAGES");
               env_lc_messages != nullptr && *env_lc_messages != L'\0') {
      for (auto locale : special_locale) {
        if (env_lc_messages == locale)
          return (char *)msgid;
      }
    }

    auto [header, size] = get_mo_data(domainname);
    if (!header)
      return (char *)msgid;

    auto data = (char *)header;
    auto o_table = (mo_string *)(data + header->O);
    auto t_table = (mo_string *)(data + header->T);

    auto compare = [data, o_table, t_table](mo_string elem, const char *value) {
      char *str = data + elem.offset;
      return strcmp(str, value) < 0;
    };

    auto it = std::lower_bound(o_table, o_table + header->N, msgid, compare);
    char *o_str = data + it->offset;
    if (strcmp(o_str, msgid) != 0)
      return (char *)msgid;

    // found
    uint32_t idx = it - o_table;
    char *t_str = data + t_table[idx].offset;
    if (GetACP() == CP_UTF8)
      return t_str;
    else {
      static wchar_t wbuf[1024];
      int wlen = MultiByteToWideChar(
          CP_UTF8, MB_ERR_INVALID_CHARS, t_str, -1, wbuf, std::size(wbuf));
      if (wlen == 0)
        return (char *)msgid;

      static char abuf[1024];
      BOOL fail = 0;
      int alen = WideCharToMultiByte(
          CP_ACP, 0, wbuf, wlen, abuf, std::size(abuf), nullptr, &fail);
      if (fail || alen == 0)
        return (char *)msgid;
      return abuf;
    }
  }
} // namespace intl
