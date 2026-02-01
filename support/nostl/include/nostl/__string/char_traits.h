#pragma once

#include <string.h>
#include <wchar.h>

#ifdef NOSTL_NOCRT
  #include <nocrt/string.h>
  #include <nocrt/wchar.h>
#endif

#include "../__config.h"

namespace NS_NOSTL
{
  namespace detail
  {
    template <typename CharT, class Int>
    struct char_traits_base
    {
    public:
      using char_type = CharT;
      using int_type = Int;

    public:
      constexpr static bool eq(char_type a, char_type b)
      {
        return a == b;
      }

      constexpr static bool lt(char_type a, char_type b)
      {
        return a < b;
      }

    protected:
      static constexpr char_type *
      consteval_move(char_type *dest, const char_type *src, size_t count)
      {
        if (dest < src) {
          for (size_t i = 0; i < count; ++i)
            dest[i] = src[i];
        } else {
          for (size_t i = count; i > 0; --i)
            dest[i - 1] = src[i - 1];
        }
        return dest;
      }

      static constexpr int
      consteval_compare(const char_type *s1, const char_type *s2, size_t count)
      {
        for (size_t i = 0; i < count; ++i) {
          if (s1[i] != s2[i])
            return s1[i] < s2[i] ? -1 : 1;
        }
        return 0;
      }

      static constexpr size_t consteval_length(const char_type *s)
      {
        size_t len = 0;
        while (s[len] != char_type(0))
          ++len;
        return len;
      }
    };
  } // namespace detail

  template <class CharT>
  class char_traits;

  template <>
  class char_traits<char> : public detail::char_traits_base<char, int>
  {
  public:
    constexpr static bool eq(char_type a, char_type b)
    {
      return static_cast<unsigned char>(a) == static_cast<unsigned char>(b);
    }

    constexpr static bool lt(char_type a, char_type b)
    {
      return static_cast<unsigned char>(a) < static_cast<unsigned char>(b);
    }

    static constexpr_if_consteval char_type *
    move(char_type *dest, const char_type *src, size_t count)
    {
      if_consteval
      {
        return consteval_move(dest, src, count);
      }
      else
      {
        return static_cast<char *>(NS_NOSTL_CRT::memmove(dest, src, count));
      }
    }

    static constexpr_if_consteval char_type *
    copy(char_type *dest, const char_type *src, size_t count)
    {
      if_consteval
      {
        return consteval_move(dest, src, count);
      }
      else
      {
        return static_cast<char *>(NS_NOSTL_CRT::memcpy(dest, src, count));
      }
    }

    static constexpr_if_consteval int
    compare(const char_type *s1, const char_type *s2, size_t count)
    {
      if_consteval
      {
        for (size_t i = 0; i < count; ++i) {
          unsigned char c1 = static_cast<unsigned char>(s1[i]);
          unsigned char c2 = static_cast<unsigned char>(s2[i]);
          if (c1 != c2)
            return c1 < c2 ? -1 : 1;
        }
        return 0;
      }
      else
      {
        return NS_NOSTL_CRT::memcmp(s1, s2, count);
      }
    }

    static constexpr_if_consteval size_t length(const char_type *s)
    {
      if_consteval
      {
        return consteval_length(s);
      }
      else
      {
        return NS_NOSTL_CRT::strlen(s);
      }
    }
  };

  template <>
  class char_traits<wchar_t> : public detail::char_traits_base<wchar_t, wint_t>
  {
  public:
    static constexpr_if_consteval char_type *
    move(char_type *dest, const char_type *src, size_t count)
    {
      if_consteval
      {
        return consteval_move(dest, src, count);
      }
      else
      {
        return NS_NOSTL_CRT::wmemmove(dest, src, count);
      }
    }

    static constexpr_if_consteval char_type *
    copy(char_type *dest, const char_type *src, size_t count)
    {
      if_consteval
      {
        return consteval_move(dest, src, count);
      }
      else
      {
        return NS_NOSTL_CRT::wmemcpy(dest, src, count);
      }
    }

    static constexpr_if_consteval int
    compare(const char_type *s1, const char_type *s2, size_t count)
    {
      if_consteval
      {
        return consteval_compare(s1, s2, count);
      }
      else
      {
        return NS_NOSTL_CRT::wmemcmp(s1, s2, count);
      }
    }

    static constexpr_if_consteval size_t length(const char_type *s)
    {
      if_consteval
      {
        return consteval_length(s);
      }
      else
      {
        return NS_NOSTL_CRT::wcslen(s);
      }
    }
  };
} // namespace NS_NOSTL
