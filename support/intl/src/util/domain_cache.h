#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "basic_string_view.h"
#include "file.h"
#include "lower_bound.h"
#include "owning_ptr.h"
#include "raw_data.h"
#include "utf8.h"

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

  struct domain_cache
  {
  private:
    struct cache_entry
    {
      string_view key;
      owning_ptr<char[]> value;

      cache_entry() = default;

      cache_entry(string_view key, owning_ptr<char[]> &&value)
          : key(key), value(static_cast<owning_ptr<char[]> &&>(value))
      {
      }

      cache_entry(const cache_entry &other) = delete;
      cache_entry &operator=(const cache_entry &other) = delete;
      cache_entry(cache_entry &&other) = default;
      cache_entry &operator=(cache_entry &&other) = default;
      ~cache_entry() = default;

      void *operator new(size_t, void *ptr)
      {
        return ptr;
      }
    };

  private:
    raw_data data_;

  private:
    cache_entry *cache_;
    size_t cache_size_;
    size_t cache_capacity_;

  public:
    domain_cache() : cache_(nullptr), cache_size_(0), cache_capacity_(0)
    {
    }

    ~domain_cache()
    {
      for (size_t i = 0; i < cache_size_; ++i)
        cache_[i].~cache_entry();
      free(cache_);
    }

    domain_cache(const domain_cache &other) = delete;
    domain_cache &operator=(const domain_cache &other) = delete;

    domain_cache(domain_cache &&other)
        : data_(static_cast<raw_data &&>(other.data_))
        , cache_(other.cache_)
        , cache_size_(other.cache_size_)
        , cache_capacity_(other.cache_capacity_)
    {
      other.cache_ = nullptr;
      other.cache_size_ = 0;
      other.cache_capacity_ = 0;
    }

    domain_cache &operator=(domain_cache &&other)
    {
      data_ = static_cast<raw_data &&>(other.data_);

      // intentionally not check self move assignment
      for (size_t i = 0; i < cache_size_; ++i)
        cache_[i].~cache_entry();
      free(cache_);

      cache_ = other.cache_;
      cache_size_ = other.cache_size_;
      cache_capacity_ = other.cache_capacity_;

      other.cache_ = nullptr;
      other.cache_size_ = 0;
      other.cache_capacity_ = 0;

      return *this;
    }

    owning_ptr<char[]> *find_local(string_view key)
    {
      size_t lo = 0, hi = cache_size_;
      while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (cache_[mid].key < key)
          lo = mid + 1;
        else
          hi = mid;
      }

      if (lo < cache_size_ && cache_[lo].key == key)
        return &cache_[lo].value;

      return nullptr;
    }

    owning_ptr<char[]> *insert_local(string_view key)
    {
      if (cache_size_ == cache_capacity_) {
        size_t new_cap = cache_capacity_ == 0 ? 16 : cache_capacity_ * 2;
        cache_entry *new_entries =
            (cache_entry *)malloc(new_cap * sizeof(cache_entry));
        if (!new_entries)
          return nullptr;

        for (size_t i = 0; i < cache_size_; i++) {
          new (&new_entries[i])
              cache_entry(static_cast<cache_entry &&>(cache_[i]));
          cache_[i].~cache_entry();
        }

        free(cache_);
        cache_ = new_entries;
        cache_capacity_ = new_cap;
      }

      size_t pos = cache_size_;
      while (pos > 0 && key < cache_[pos - 1].key) {
        new (&cache_[pos])
            cache_entry(static_cast<cache_entry &&>(cache_[pos - 1]));
        cache_[pos - 1].~cache_entry();
        --pos;
      }

      new (&cache_[pos]) cache_entry();
      cache_[pos].key = string_view(key.data(), key.size());
      ++cache_size_;

      return &cache_[pos].value;
    }

  public:
    char *gettext(string_view msgid)
    {
      if (!data_)
        return (char *)msgid.data();

      mo_header *header = data_.as<mo_header>();
      mo_string *o_table = (data_ + header->O).as<mo_string>();
      mo_string *t_table = (data_ + header->T).as<mo_string>();

      auto compare = [this](mo_string elem, string_view msgid) {
        string_view s = {data_ + elem.offset, elem.length};
        return s < msgid;
      };

      auto it = intl::lower_bound(o_table, o_table + header->N, msgid, compare);
      if (it == o_table + header->N)
        return (char *)msgid.data();

      size_t idx = it - o_table;
      string_view o_sv = {data_ + o_table[idx].offset, o_table[idx].length};
      if (o_sv != msgid)
        return (char *)msgid.data();

      char *t = data_ + t_table[idx].offset;
      size_t length = t_table[idx].length;

      if (system_is_utf8())
        return t;

      if (owning_ptr<char[]> *val = find_local(msgid))
        if (val)
          return val->observe();

      int wlen = MultiByteToWideChar(CP_UTF8, 0, t, length, nullptr, 0);
      if (wlen == 0)
        return (char *)msgid.data();
      owning_ptr<wchar_t[]> wbuf(
          (wchar_t *)malloc((wlen + 1) * sizeof(wchar_t)));
      if (!wbuf)
        return (char *)msgid.data();
      MultiByteToWideChar(CP_UTF8, 0, t, length, wbuf.observe(), wlen);
      wbuf[wlen] = 0;

      BOOL fail;
      int alen = WideCharToMultiByte(
          CP_ACP, 0, wbuf.observe(), wlen, nullptr, 0, nullptr, &fail);
      if (alen == 0 || fail)
        return (char *)msgid.data();

      owning_ptr<char[]> abuf((char *)malloc(alen + 1));
      if (!abuf)
        return (char *)msgid.data();
      WideCharToMultiByte(CP_ACP,
                          0,
                          wbuf.observe(),
                          wlen,
                          abuf.observe(),
                          alen,
                          nullptr,
                          nullptr);
      abuf[alen] = 0;

      if (owning_ptr<char[]> *val = insert_local(msgid)) {
        *val = static_cast<owning_ptr<char[]> &&>(abuf);
        return val->observe();
      }

      return (char *)msgid.data();
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
        return result;
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
      result.data_ = static_cast<raw_data &&>(data);
      return result;
    }
  };
} // namespace intl
