#pragma once

#include <stdlib.h>
#include <wchar.h>

#include "domain_cache.h"

namespace intl
{
  struct cache_manager_t
  {
  private:
    struct entry
    {
      owning_ptr<char> key;
      domain_cache value;

      entry() = default;

      explicit entry(owning_ptr<char> &&key)
          : key(static_cast<owning_ptr<char> &&>(key))
      {
      }

      entry(owning_ptr<char> &&key, domain_cache &&value)
          : key(static_cast<owning_ptr<char> &&>(key))
          , value(static_cast<domain_cache &&>(value))
      {
      }

      entry(const entry &other) = delete;

      entry(entry &&other) = default;

      entry &operator=(const entry &other) = delete;

      entry &operator=(entry &&other) = default;

      ~entry() = default;

      void *operator new(size_t, void *ptr)
      {
        return ptr;
      }
    };

  private:
    entry *data_;
    size_t size_;
    size_t capacity_;

  public:
    cache_manager_t() : data_(nullptr), size_(0), capacity_(0)
    {
    }

    ~cache_manager_t()
    {
      for (size_t i = 0; i < size_; ++i)
        data_[i].~entry();
      free(data_);
    }

    cache_manager_t(const cache_manager_t &) = delete;
    cache_manager_t &operator=(const cache_manager_t &) = delete;

    cache_manager_t(cache_manager_t &&other)
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
      other.data_ = nullptr;
      other.size_ = 0;
      other.capacity_ = 0;
    }

    cache_manager_t &operator=(cache_manager_t &&other)
    {
      // intentionally not check self move assignment
      for (size_t i = 0; i < size_; ++i)
        data_[i].~entry();
      free(data_);

      data_ = other.data_;
      size_ = other.size_;
      capacity_ = other.capacity_;

      other.data_ = nullptr;
      other.size_ = 0;
      other.capacity_ = 0;

      return *this;
    }

    domain_cache *get_or_null(string_view domain)
    {
      for (size_t i = 0; i < size_; ++i) {
        string_view key(data_[i].key.observe());
        if (key == domain)
          return &data_[i].value;
      }
      return nullptr;
    }

    domain_cache *allocate_slot(owning_ptr<char> &&domain)
    {
      if (size_ == capacity_) {
        size_t new_cap = capacity_ ? capacity_ * 2 : 1;
        entry *new_data = (entry *)malloc(new_cap * sizeof(entry));
        if (!new_data)
          return nullptr;

        for (size_t i = 0; i < size_; i++) {
          new (&new_data[i]) entry(static_cast<entry &&>(data_[i]));
          data_[i].~entry();
        }

        free(data_);
        data_ = new_data;
        capacity_ = new_cap;
      }

      new (&data_[size_]) entry(static_cast<owning_ptr<char> &&>(domain));
      return &data_[size_++].value;
    }

    domain_cache *get_or_load(string_view domain)
    {
      if (domain_cache *val = get_or_null(domain))
        return val;

      owning_ptr<wchar_t> prefix = resolve_prefix();
      if (!prefix)
        return nullptr;

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
      wstring_view prefix_sv(prefix.observe());
      if (env_language != nullptr && *env_language != L'\0')
        fp = find_locale_file_by_env_language(prefix_sv, env_language, domain);
      else if (env_lc_all != nullptr && *env_lc_all != L'\0')
        fp = find_locale_file(prefix_sv, env_lc_all, domain);
      else if (env_lc_messages != nullptr && *env_lc_messages != L'\0')
        fp = find_locale_file(prefix_sv, env_lc_messages, domain);
      else if (env_lang != nullptr && *env_lang != L'\0')
        fp = find_locale_file(prefix_sv, env_lang, domain);
      else
        fp = find_locale_file_from_win32(prefix_sv, domain);

      if (!fp)
        return nullptr;

      owning_ptr<char> domain_copy((char *)malloc(domain.size() + 1));
      if (!domain_copy)
        return nullptr;
      memcpy(domain_copy.observe(), domain.data(), domain.size());
      domain_copy.observe()[domain.size()] = '\0';

      domain_cache entry = domain_cache::from_file(static_cast<file &&>(fp));
      if (domain_cache *val =
              allocate_slot(static_cast<owning_ptr<char> &&>(domain_copy))) {
        *val = static_cast<domain_cache &&>(entry);
        return val;
      }
      return nullptr;
    }

  private:
    owning_ptr<wchar_t> resolve_prefix()
    {
      wchar_t *path = (wchar_t *)malloc(MAX_PATH * sizeof(wchar_t));
      if (!path)
        return nullptr;

      DWORD exe_len = GetModuleFileNameW(nullptr, path, MAX_PATH);
      if (exe_len >= MAX_PATH || exe_len == 0) {
        free(path);
        return nullptr;
      }

      const wchar_t *target = LR"(\lib\gcc\)";
      wchar_t *pos = wcsstr(path, target);
      if (!pos) {
        target = LR"(\bin\)";
        pos = wcsstr(path, target);
      }
      if (!pos) {
        free(path);
        return nullptr;
      }

      *pos = L'\0';

      if (wcscat_s(path, MAX_PATH, LR"(\share\locale\)") != 0) {
        free(path);
        return nullptr;
      }

      return owning_ptr<wchar_t>(path);
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
        owning_ptr<wchar_t[]> gnu_name(
            (wchar_t *)malloc((language_name.size() + 1) * sizeof(wchar_t)));
        if (!gnu_name)
          return {};
        wmemcpy(gnu_name.observe(), language_name.data(), language_name.size());
        gnu_name[language_name.size()] = L'\0';
        gnu_name[pos1] = L'_';
        file result = find_locale_file(
            prefix, {gnu_name.observe(), language_name.size()}, domain);
        return result;
      } else {
        // ll-Script-CC
        // Win32 script name differs from GNU ones, ignore it.
        wstring_view region_code = language_name.substr(pos2 + 1);
        owning_ptr<wchar_t[]> gnu_name((wchar_t *)malloc(
            (pos1 + 1 + region_code.size() + 1) * sizeof(wchar_t)));
        if (!gnu_name)
          return {};
        wmemcpy(gnu_name.observe(), language_name.data(), pos1);
        gnu_name[pos1] = L'_';
        wmemcpy(gnu_name.observe() + pos1 + 1,
                region_code.data(),
                region_code.size());
        gnu_name[pos1 + 1 + region_code.size()] = L'\0';
        file result = find_locale_file(
            prefix,
            {gnu_name.observe(), pos1 + 1 + region_code.size()},
            domain);
        return result;
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
      owning_ptr<wchar_t> path =
          compose_locale_file_path(prefix, locale, domain);
      if (!path)
        return {};

      file fp(path.observe(), L"rb");
      if (fp)
        return fp;

      size_t pos = locale.find_first_of(L".@");
      if (pos != wstring_view::npos) {
        path = compose_locale_file_path(prefix, locale.substr(0, pos), domain);
        if (!path)
          return {};

        fp = file(path.observe(), L"rb");
        if (fp)
          return fp;
      }

      pos = locale.find(L'_');
      if (pos != wstring_view::npos) {
        path = compose_locale_file_path(prefix, locale.substr(0, pos), domain);
        if (!path)
          return {};

        fp = file(path.observe(), L"rb");
        if (fp)
          return fp;
      }

      return {};
    }

    owning_ptr<wchar_t> compose_locale_file_path(wstring_view prefix,
                                                 wstring_view locale,
                                                 string_view domain)
    {
      const wchar_t *infix = LR"(\LC_MESSAGES\)";
      const wchar_t *suffix = L".mo";
      size_t infix_len = wcslen(infix);
      size_t suffix_len = wcslen(suffix);

      size_t total_len = prefix.size() + locale.size() + infix_len +
                         domain.size() + suffix_len + 1;

      wchar_t *result = (wchar_t *)malloc(total_len * sizeof(wchar_t));
      if (!result)
        return nullptr;

      wchar_t *p = result;

      wmemcpy(p, prefix.data(), prefix.size());
      p += prefix.size();

      wmemcpy(p, locale.data(), locale.size());
      p += locale.size();

      wmemcpy(p, infix, infix_len);
      p += infix_len;

      for (size_t i = 0; i < domain.size(); ++i)
        *p++ = (wchar_t)(unsigned char)domain[i];

      wmemcpy(p, suffix, suffix_len);
      p += suffix_len;

      *p = L'\0';

      return owning_ptr<wchar_t>(result);
    }
  };
} // namespace intl
