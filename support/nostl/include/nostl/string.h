#pragma once

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>

#include "__config.h"

#include "__iterator/reverse_iterator.h"
#include "__string/char_traits.h"
#include "string_view.h"

#ifdef NOSTL_NOCRT
  #include <nocrt/stdlib.h>
#endif

namespace NS_NOSTL
{
  namespace detail
  {
    template <size_t N>
    struct sso_size_t
    {
      char padding[N];
      unsigned char size : 7;
      unsigned char is_heap : 1;

      sso_size_t() : padding(), size(0), is_heap(0)
      {
      }
    };

    template <>
    struct sso_size_t<0>
    {
      unsigned char size : 7;
      unsigned char is_heap : 1;

      sso_size_t() : size(0), is_heap(0)
      {
      }
    };
  } // namespace detail

  template <class CharT, class Traits = char_traits<CharT>>
  class basic_string;

  using string = basic_string<char>;
  using wstring = basic_string<wchar_t>;
#ifdef __cpp_char8_t
  using u8string = basic_string<char8_t>;
#endif
  using u16string = basic_string<char16_t>;
  using u32string = basic_string<char32_t>;

  template <class CharT, class Traits>
  class basic_string
  {
  public:
    using traits_type = Traits;
    using value_type = CharT;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = value_type *;
    using const_iterator = const value_type *;
    using reverse_iterator = NS_NOSTL::reverse_iterator<iterator>;
    using const_reverse_iterator = NS_NOSTL::reverse_iterator<const_iterator>;

  public:
    static constexpr size_type npos = size_type(-1);

  private:
    struct heap_t
    {
      pointer data;
      size_type size;
      size_type capacity : sizeof(size_type) * CHAR_BIT - 1;
      size_type is_heap : 1;
    };

    static constexpr size_t sso_capacity =
        (sizeof(heap_t) - 1) / sizeof(value_type);

    struct sso_t
    {
      value_type data[sso_capacity];
      detail::sso_size_t<sizeof(value_type) - 1> size;

      sso_t() : data(), size() {};
    };

    union storage_t
    {
      heap_t heap;
      sso_t sso;

      storage_t() : sso()
      {
      }
    };

    storage_t storage;

  public:
    static constexpr size_t max_sso_size()
    {
      return sso_capacity - 1;
    }

  public:
    constexpr basic_string() : storage()
    {
    }

    basic_string(size_type count, CharT ch)
    {
      if (fits_in_sso(count)) {
        storage.sso.size.size = count;
        storage.sso.size.is_heap = 0;

        for (size_t i = 0; i < count; ++i)
          storage.sso.data[i] = ch;
        storage.sso.data[count] = value_type(0);
      } else {
        size_type alloc_size = aligned_alloc_size(count);
        value_type *data = static_cast<value_type *>(
            NS_NOSTL_CRT::malloc(alloc_size * sizeof(value_type)));
        if (!data)
          NOSTL_RAISE_BAD_ALLOC();

        for (size_t i = 0; i < count; ++i)
          data[i] = ch;
        data[count] = value_type(0);

        storage.heap.data = data;
        storage.heap.size = count;
        storage.heap.capacity = alloc_size - 1;
        storage.heap.is_heap = 1;
      }
    }

    constexpr basic_string(const CharT *s, size_type count)
    {
      if (fits_in_sso(count)) {
        storage.sso.size.size = count;
        storage.sso.size.is_heap = 0;

        Traits::copy(storage.sso.data, s, count);
        storage.sso.data[count] = value_type(0);
      } else {
        size_type alloc_size = aligned_alloc_size(count);
        value_type *data = static_cast<value_type *>(
            NS_NOSTL_CRT::malloc(alloc_size * sizeof(value_type)));
        if (!data)
          NOSTL_RAISE_BAD_ALLOC();

        Traits::copy(data, s, count);
        data[count] = value_type(0);

        storage.heap.data = data;
        storage.heap.size = count;
        storage.heap.capacity = alloc_size - 1;
        storage.heap.is_heap = 1;
      }
    }

    constexpr basic_string(const CharT *s) : basic_string(s, Traits::length(s))
    {
    }

    constexpr basic_string(const basic_string &other)
        : basic_string(other.data(), other.size())
    {
    }

    constexpr basic_string(basic_string &&other) : storage(other.storage)
    {
      other.storage.sso = sso_t();
    }

    ~basic_string()
    {
      free_heap();
    }

    basic_string &operator=(const basic_string &str)
    {
      if (this == &str)
        return *this;
      basic_string tmp(str);
      swap(tmp);
      return *this;
    }

    basic_string &operator=(basic_string &&str)
    {
      // intentionally not check self move assignment
      free_heap();
      storage = str.storage;
      str.storage.sso = sso_t();
      return *this;
    }

    // element access
  public:
    constexpr CharT &operator[](size_type pos)
    {
      return data()[pos];
    }

    constexpr const CharT &operator[](size_type pos) const
    {
      return data()[pos];
    }

    constexpr CharT &front()
    {
      return data()[0];
    }

    constexpr const CharT &front() const
    {
      return data()[0];
    }

    constexpr CharT &back()
    {
      return data()[size() - 1];
    }

    constexpr const CharT &back() const
    {
      return data()[size() - 1];
    }

    constexpr const CharT *data() const
    {
      return is_heap() ? storage.heap.data : storage.sso.data;
    }

    constexpr CharT *data()
    {
      return is_heap() ? storage.heap.data : storage.sso.data;
    }

    constexpr const CharT *c_str() const
    {
      return data();
    }

    constexpr operator basic_string_view<CharT, Traits>() const
    {
      return basic_string_view<CharT, Traits>(data(), size());
    }

    // capacity
  public:
    constexpr bool empty() const
    {
      return size() == 0;
    }

    constexpr size_type size() const
    {
      return is_heap() ? storage.heap.size : storage.sso.size.size;
    }

    constexpr size_type length() const
    {
      return size();
    }

    // modifiers
  public:
    void clear()
    {
      if (is_heap()) {
        storage.heap.data[0] = value_type(0);
        storage.heap.size = 0;
      } else {
        storage.sso.data[0] = value_type(0);
        storage.sso.size.size = 0;
      }
    }

    constexpr void push_back(CharT ch)
    {
      append(1, ch);
    }

    constexpr basic_string &append(const CharT *s, size_type count)
    {
      size_type old_size = size();
      resize(old_size + count);
      Traits::copy(data() + old_size, s, count);
      return *this;
    }

    constexpr basic_string &append(size_type count, CharT ch)
    {
      resize(size() + count, ch);
      return *this;
    }

    template <class SV>
    constexpr basic_string &append(const SV &t)
    {
      basic_string_view<CharT, Traits> sv(t);
      return append(sv.data(), sv.size());
    }

    template <class StringViewLike>
    constexpr basic_string &operator+=(const StringViewLike &t)
    {
      basic_string_view<CharT, Traits> sv(t);
      return append(sv.data(), sv.size());
    }

    constexpr void resize(size_type count)
    {
      resize(count, CharT());
    }

    void resize(size_type count, CharT ch)
    {
      if (is_heap()) {
        if (count <= storage.heap.capacity) {
          for (size_t i = size(); i < count; ++i)
            storage.heap.data[i] = ch;
          storage.heap.data[count] = value_type(0);
          storage.heap.size = count;
        } else {
          size_t alloc_size = aligned_alloc_size(count);
          value_type *new_data =
              static_cast<value_type *>(NS_NOSTL_CRT::realloc(
                  storage.heap.data, alloc_size * sizeof(value_type)));
          if (!new_data)
            NOSTL_RAISE_BAD_ALLOC();

          for (size_t i = size(); i < count; ++i)
            new_data[i] = ch;
          new_data[count] = value_type(0);

          storage.heap.data = new_data;
          storage.heap.size = count;
          storage.heap.capacity = alloc_size - 1;
        }
      } else {
        if (fits_in_sso(count)) {
          for (size_t i = size(); i < count; ++i)
            storage.sso.data[i] = ch;
          storage.sso.data[count] = value_type(0);
          storage.sso.size.size = count;
        } else {
          size_t alloc_size = aligned_alloc_size(count);
          value_type *new_data = static_cast<value_type *>(
              NS_NOSTL_CRT::malloc(alloc_size * sizeof(value_type)));
          if (!new_data)
            NOSTL_RAISE_BAD_ALLOC();

          Traits::copy(new_data, storage.sso.data, size());
          for (size_t i = size(); i < count; ++i)
            new_data[i] = ch;
          new_data[count] = value_type(0);

          storage.heap.data = new_data;
          storage.heap.size = count;
          storage.heap.capacity = alloc_size - 1;
          storage.heap.is_heap = 1;
        }
      }
    }

    constexpr void swap(basic_string &other)
    {
      storage_t tmp = storage;
      storage = other.storage;
      other.storage = tmp;
    }

    constexpr size_type rfind(CharT ch, size_type pos = npos) const
    {
      if (empty())
        return npos;
      if (pos == npos || pos >= size())
        pos = size() - 1;
      for (size_type i = 0; i <= pos; ++i) {
        size_type idx = pos - i;
        if (Traits::eq(data()[idx], ch))
          return idx;
      }
      return npos;
    }

    template <class StringViewLike>
    constexpr size_type rfind(const StringViewLike &t,
                              size_type pos = npos) const
    {
      basic_string_view<CharT, Traits> sv = t;
      if (sv.empty())
        return pos < size() ? pos : size();
      if (sv.size() > size())
        return npos;

      if (pos == npos || pos > size() - sv.size())
        pos = size() - sv.size();
      for (size_type i = 0; i <= pos; ++i) {
        size_type idx = pos - i;
        if (Traits::compare(data() + idx, sv.data(), sv.size()) == 0)
          return idx;
      }
      return npos;
    }

    int compare(const basic_string &str) const
    {
      size_type rlen = size() < str.size() ? size() : str.size();
      int res = Traits::compare(data(), str.data(), rlen);
      if (res != 0)
        return res;
      else if (size() < str.size())
        return -1;
      else if (size() > str.size())
        return 1;
      else
        return 0;
    }

  private:
    constexpr bool is_heap() const
    {
      return storage.heap.is_heap;
    }

    constexpr void free_heap()
    {
      if (is_heap())
        NS_NOSTL_CRT::free(storage.heap.data);
    }

  private:
    static constexpr bool fits_in_sso(size_type count)
    {
      return count < sso_capacity;
    }

    static constexpr size_type alignment = 16 / sizeof(value_type);

    static constexpr size_type aligned_alloc_size(size_type count)
    {
      size_t required_size = count + 1;
      size_t aligned_size = (required_size + alignment - 1) & ~(alignment - 1);
      return aligned_size;
    }
  };

  template <class CharT, class Traits>
  constexpr bool operator==(const basic_string<CharT, Traits> &lhs,
                            const basic_string<CharT, Traits> &rhs) noexcept
  {
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
  }

  template <class CharT, class Traits>
  constexpr bool operator!=(const basic_string<CharT, Traits> &lhs,
                            const basic_string<CharT, Traits> &rhs) noexcept
  {
    return !(lhs == rhs);
  }

  template <class CharT, class Traits>
  constexpr bool operator<(const basic_string<CharT, Traits> &lhs,
                           const basic_string<CharT, Traits> &rhs) noexcept
  {
    return lhs.compare(rhs) < 0;
  }

  template <class CharT, class Traits>
  constexpr bool operator<=(const basic_string<CharT, Traits> &lhs,
                            const basic_string<CharT, Traits> &rhs) noexcept
  {
    return !(rhs < lhs);
  }

  template <class CharT, class Traits>
  constexpr bool operator>(const basic_string<CharT, Traits> &lhs,
                           const basic_string<CharT, Traits> &rhs) noexcept
  {
    return rhs < lhs;
  }

  template <class CharT, class Traits>
  constexpr bool operator>=(const basic_string<CharT, Traits> &lhs,
                            const basic_string<CharT, Traits> &rhs) noexcept
  {
    return !(lhs < rhs);
  }
} // namespace NS_NOSTL
