#include "../win32/utf8_buffer.h"

#include <thunk/string.h>
#include <thunk/unicode.h>
#include <thunk/utf8-musl.h>

#include <io.h>
#include <string.h>

namespace mingw_thunk
{
  namespace musl
  {
    ssize_t read(int fd, void *buf, size_t count)
    {
      if (count == 0)
        return 0;

      HANDLE h = (HANDLE)_get_osfhandle(fd);
      if (h == INVALID_HANDLE_VALUE)
        return -1;

      utf8_buffer &u8 = g_utf8_buffer[fd];
      unsigned char *out = (unsigned char *)buf;
      unsigned total = 0;

      if (u8.len > 0) {
        int n = u8.len;
        if (n > (int)count)
          n = (int)count;
        memcpy(out, u8.buf, n);
        u8.len -= n;
        if (u8.len > 0)
          memmove(u8.buf, u8.buf + n, u8.len);
        total = (unsigned)n;
        if (total == count)
          return (int)total;
      }

      wchar_t wbuf[4096];
      bool read_finish = false;

      while (total < count && !read_finish) {
        unsigned space = count - total;
        unsigned want = space / 3 + 1;
        if (want > 4096)
          want = 4096;

        DWORD wread = 0;
        if (!ReadConsoleW(h, wbuf, want, &wread, nullptr))
          return total > 0 ? (int)total : -1;

        if (wread == 0)
          break;

        if (wread > 0 && i::u16_is_high(wbuf[wread - 1])) {
          wchar_t low;
          DWORD wread2 = 0;
          if (ReadConsoleW(h, &low, 1, &wread2, nullptr) && wread2 == 1 &&
              i::u16_is_low(low)) {
            wbuf[wread] = low;
            wread++;
          } else {
            wread--;
          }
        }

        wchar_t *dst = wbuf;
        for (unsigned i = 0; i < wread; i++) {
          if (wbuf[i] == 0x0004) {
            // ^D
            read_finish = true;
            break;
          }
          if (wbuf[i] == 0x000a) {
            // \n
            read_finish = true;
          }
          if (wbuf[i] == 0x000d) {
            // \r
            continue;
          }
          *dst++ = wbuf[i];
        }
        unsigned valid = (unsigned)(dst - wbuf);
        if (valid == 0)
          break;

        d::u_str utf8;
        if (!utf8.from_w(wbuf, (int)valid))
          return total > 0 ? (int)total : -1;

        unsigned remaining = count - total;
        unsigned to_copy = (unsigned)utf8.size();
        if (to_copy > remaining)
          to_copy = remaining;
        memcpy(out + total, utf8.data(), to_copy);
        total += to_copy;

        if ((unsigned)utf8.size() > remaining) {
          unsigned excess = (unsigned)utf8.size() - remaining;
          if (excess <= 4) {
            memcpy(
                u8.buf, (const unsigned char *)utf8.data() + to_copy, excess);
            u8.len = (int)excess;
          }
          break;
        }
      }

      return total;
    }
  } // namespace musl
} // namespace mingw_thunk
