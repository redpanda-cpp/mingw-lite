#pragma once

#include "_no_thunk.h"
#include "buffer.h"

#include <stddef.h>
#include <stringapiset.h>

namespace mingw_thunk
{
  namespace c
  {
    inline void *memcpy(void *dest, const void *src, size_t count) noexcept
    {
      char *d = static_cast<char *>(dest);
      const char *s = static_cast<const char *>(src);
      for (size_t i = 0; i < count; i++)
        d[i] = s[i];
      return dest;
    }

    inline void *memmove(void *dest, const void *src, size_t count) noexcept
    {
      char *d = static_cast<char *>(dest);
      const char *s = static_cast<const char *>(src);
      if (d < s)
        for (size_t i = 0; i < count; i++)
          d[i] = s[i];
      else
        for (size_t i = count; i > 0; i--)
          d[i - 1] = s[i - 1];
      return dest;
    }

    inline void *memset(void *dest, int ch, size_t count) noexcept
    {
      char *d = static_cast<char *>(dest);
      for (size_t i = 0; i < count; i++)
        d[i] = char(ch);
      return dest;
    }

    inline char *stpncpy(char *dst, const char *src, size_t dsize)
    {
      char *dend = dst + dsize;
      while (dst < dend && *src)
        *dst++ = *src++;
      char *ret = dst;
      while (dst < dend)
        *dst++ = 0;
      return ret;
    }

    inline char *strcpy(char *dest, const char *src)
    {
      char *cur = dest;
      while (*src)
        *cur++ = *src++;
      *cur = 0;
      return dest;
    }

    inline size_t strlen(const char *str) noexcept
    {
      size_t len = 0;
      while (str[len] != 0)
        len++;
      return len;
    }

    inline wchar_t *wcschr(const wchar_t *wcs, wchar_t wc)
    {
      while (*wcs) {
        if (*wcs == wc)
          return const_cast<wchar_t *>(wcs);
        wcs++;
      }
      return nullptr;
    }

    inline wchar_t *wcscpy(wchar_t *dest, const wchar_t *src)
    {
      wchar_t *cur = dest;
      while (*src)
        *cur++ = *src++;
      *cur = 0;
      return dest;
    }

    inline size_t wcslen(const wchar_t *str) noexcept
    {
      size_t len = 0;
      while (str[len] != 0)
        len++;
      return len;
    }

    inline size_t wcsnlen(const wchar_t *str, size_t strsz)
    {
      size_t len = 0;
      while (len < strsz && str[len] != 0)
        len++;
      return len;
    }

    inline wchar_t *
    wmemcpy(wchar_t *dest, const wchar_t *src, size_t count) noexcept
    {
      for (size_t i = 0; i < count; i++)
        dest[i] = src[i];
      return dest;
    }
  } // namespace c

  namespace i
  {
    template <class DstChar, class SrcChar>
    inline bool best_effort_convert(DstChar *dest,
                                    int dest_size,
                                    const SrcChar *src,
                                    int src_len,
                                    int (*converter)(DstChar *dest,
                                                     int dest_size,
                                                     const SrcChar *src,
                                                     int src_len)) noexcept
    {
      if (dest_size == 0 || src_len < 0) {
        return false;
      }

      if (src_len == 0) {
        dest[0] = DstChar();
        return true;
      }

      int available_size = dest_size - 1;

      int result = converter(dest, available_size, src, src_len);
      if (result > 0) {
        dest[result] = DstChar();
        return true;
      }

      int left = 0;
      int right = src_len;
      int best_src_len = 0;

      while (left <= right) {
        int mid = left + (right - left) / 2;
        if (mid == 0)
          break;

        int test = converter(dest, available_size, src, mid);
        if (test > 0) {
          best_src_len = mid;
          left = mid + 1;
        } else {
          right = mid - 1;
        }
      }

      if (best_src_len > 0) {
        result = converter(dest, available_size, src, best_src_len);
        if (result > 0) {
          dest[result] = DstChar();
          return true;
        }
      }

      dest[0] = DstChar();
      return false;
    }
  } // namespace i

  namespace d
  {
    struct max_path_tag
    {
    };

    class a_str : public buffer<MAX_PATH, char>
    {
    public:
      a_str() noexcept = default;

      a_str(max_path_tag) noexcept : buffer<MAX_PATH, char>()
      {
        resize(MAX_PATH - 1);
      }

      const char *c_str() const noexcept
      {
        return data();
      }

      static int size_from_w(const wchar_t *str, int len) noexcept
      {
        if (len == 0)
          return 0;

        return __ms_WideCharToMultiByte(
            CP_ACP, 0, str, len, nullptr, 0, nullptr, nullptr);
      }

      static int fixed_buffer_from_w(char *dest,
                                     int dest_size,
                                     const wchar_t *src,
                                     int src_len) noexcept
      {
        if (dest_size == 0)
          return 0;

        return __ms_WideCharToMultiByte(
            CP_ACP, 0, src, src_len, dest, dest_size, nullptr, nullptr);
      }

      bool from_w(const wchar_t *str, int len) noexcept
      {
        if (len == 0) {
          this->clear();
          return true;
        }

        int required = size_from_w(str, len);
        if (required == 0)
          return false;

        if (!this->resize(required))
          return false;

        int result = fixed_buffer_from_w(this->data(), this->size(), str, len);

        if (result != required) {
          SetLastError(ERROR_INVALID_DATA);
          this->clear();
          return false;
        }

        return true;
      }

      bool from_w(const wchar_t *str) noexcept
      {
        size_t len = c::wcslen(str);
        if (len > size_t(INT_MAX))
          return false;
        return from_w(str, int(len));
      }

      static bool best_effort_from_w(char *dest,
                                     int dest_size,
                                     const wchar_t *src,
                                     int src_len) noexcept
      {
        return i::best_effort_convert(
            dest, dest_size, src, src_len, &fixed_buffer_from_w);
      }

      static bool
      best_effort_from_w(char *dest, int dest_size, const wchar_t *src) noexcept
      {
        size_t len = c::wcslen(src);
        if (len > size_t(INT_MAX))
          return false;
        return best_effort_from_w(dest, dest_size, src, int(len));
      }
    };

    class u_str : public buffer<MAX_PATH, char>
    {
    public:
      u_str() noexcept = default;

      u_str(max_path_tag) noexcept : buffer<MAX_PATH, char>()
      {
        resize(MAX_PATH - 1);
      }

      const char *c_str() const noexcept
      {
        return data();
      }

      static int size_from_w(const wchar_t *str, int len) noexcept
      {
        if (len == 0)
          return 0;

        return WideCharToMultiByte(
            CP_UTF8, 0, str, len, nullptr, 0, nullptr, nullptr);
      }

      static int fixed_buffer_from_w(char *dest,
                                     int dest_size,
                                     const wchar_t *src,
                                     int src_len) noexcept
      {
        if (dest_size == 0)
          return 0;

        return WideCharToMultiByte(
            CP_UTF8, 0, src, src_len, dest, dest_size, nullptr, nullptr);
      }

      bool from_w(const wchar_t *str, int len) noexcept
      {
        if (len == 0) {
          this->clear();
          return true;
        }

        int required = size_from_w(str, len);
        if (required == 0)
          return false;

        if (!this->resize(required))
          return false;

        int result = fixed_buffer_from_w(this->data(), this->size(), str, len);

        if (result != required) {
          SetLastError(ERROR_INVALID_DATA);
          this->clear();
          return false;
        }

        return true;
      }

      bool from_w(const wchar_t *str) noexcept
      {
        size_t len = c::wcslen(str);
        if (len > size_t(INT_MAX))
          return false;
        return from_w(str, int(len));
      }

      static bool best_effort_from_w(char *dest,
                                     int dest_size,
                                     const wchar_t *src,
                                     int src_len) noexcept
      {
        return i::best_effort_convert(
            dest, dest_size, src, src_len, &fixed_buffer_from_w);
      }

      static bool
      best_effort_from_w(char *dest, int dest_size, const wchar_t *src)
      {
        size_t len = c::wcslen(src);
        if (len > size_t(INT_MAX))
          return false;
        return best_effort_from_w(dest, dest_size, src, int(len));
      }
    };

    class w_str : public buffer<MAX_PATH, wchar_t>
    {
    public:
      w_str() noexcept = default;

      w_str(max_path_tag) noexcept : buffer<MAX_PATH, wchar_t>()
      {
        resize(MAX_PATH - 1);
      }

      const wchar_t *c_str() const noexcept
      {
        return data();
      }

      static int size_from_a(const char *str, int len) noexcept
      {
        if (len == 0)
          return 0;

        return __ms_MultiByteToWideChar(CP_ACP, 0, str, len, nullptr, 0);
      }

      static int fixed_buffer_from_a(wchar_t *dest,
                                     int dest_size,
                                     const char *src,
                                     int src_len) noexcept
      {
        if (dest_size == 0)
          return 0;

        return __ms_MultiByteToWideChar(
            CP_ACP, 0, src, src_len, dest, dest_size);
      }

      bool from_a(const char *str, int len) noexcept
      {
        if (len == 0) {
          this->clear();
          return true;
        }

        int required = size_from_a(str, len);
        if (required == 0)
          return false;

        if (!this->resize(required))
          return false;

        int result = fixed_buffer_from_a(this->data(), this->size(), str, len);

        if (result != required) {
          SetLastError(ERROR_INVALID_DATA);
          this->clear();
          return false;
        }

        return true;
      }

      bool from_a(const char *str) noexcept
      {
        size_t len = c::strlen(str);
        if (len > size_t(INT_MAX))
          return false;
        return from_a(str, int(len));
      }

      static bool best_effort_from_a(wchar_t *dest,
                                     int dest_size,
                                     const char *src,
                                     int src_len) noexcept
      {
        return i::best_effort_convert(
            dest, dest_size, src, src_len, &fixed_buffer_from_a);
      }

      static bool
      best_effort_from_a(wchar_t *dest, int dest_size, const char *src)
      {
        size_t len = c::strlen(src);
        if (len > size_t(INT_MAX))
          return false;
        return best_effort_from_a(dest, dest_size, src, int(len));
      }

      static int size_from_u(const char *str, int len) noexcept
      {
        if (len == 0)
          return 0;

        return MultiByteToWideChar(CP_UTF8, 0, str, len, nullptr, 0);
      }

      static int fixed_buffer_from_u(wchar_t *dest,
                                     int dest_size,
                                     const char *src,
                                     int src_len) noexcept
      {
        if (dest_size == 0)
          return 0;

        return MultiByteToWideChar(CP_UTF8, 0, src, src_len, dest, dest_size);
      }

      bool from_u(const char *str, int len) noexcept
      {
        if (len == 0) {
          this->clear();
          return true;
        }

        int required = size_from_u(str, len);
        if (required == 0)
          return false;

        if (!this->resize(required))
          return false;

        int result = fixed_buffer_from_u(this->data(), this->size(), str, len);

        if (result != required) {
          SetLastError(ERROR_INVALID_DATA);
          this->clear();
          return false;
        }

        return true;
      }

      bool from_u(const char *str) noexcept
      {
        size_t len = c::strlen(str);
        if (len > size_t(INT_MAX))
          return false;
        return from_u(str, int(len));
      }

      static bool best_effort_from_u(wchar_t *dest,
                                     int dest_size,
                                     const char *src,
                                     int src_len) noexcept
      {
        return i::best_effort_convert(
            dest, dest_size, src, src_len, &fixed_buffer_from_u);
      }

      static bool
      best_effort_from_u(wchar_t *dest, int dest_size, const char *src)
      {
        size_t len = c::strlen(src);
        if (len > size_t(INT_MAX))
          return false;
        return best_effort_from_u(dest, dest_size, src, int(len));
      }
    };
  } // namespace d
} // namespace mingw_thunk
