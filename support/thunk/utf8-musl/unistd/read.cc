#include "../internal/stdio_impl.h"
#include <thunk/string.h>
#include <thunk/u8crt/musl.h>
#include <thunk/unicode.h>

#include <io.h>
#include <string.h>

namespace mingw_thunk
{
  namespace musl
  {
    int read(int fd, void *buffer, unsigned int count)
    {
      if (count == 0)
        return 0;

      HANDLE h = (HANDLE)_get_osfhandle(fd);
      if (h == INVALID_HANDLE_VALUE)
        return -1;

      FILE *f = g_fp_from_fd(fd);
      unsigned char *out = (unsigned char *)buffer;
      unsigned total = 0;

      if (f && f->u8_len > 0) {
        int n = f->u8_len;
        if (n > (int)count)
          n = (int)count;
        memcpy(out, f->u8_buf, n);
        f->u8_len -= n;
        if (f->u8_len > 0)
          memmove(f->u8_buf, f->u8_buf + n, f->u8_len);
        total = (unsigned)n;
        if (total == count)
          return (int)total;
      }

      wchar_t wbuf[4096];
      bool saw_eof = false;

      while (total < count && !saw_eof) {
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
            saw_eof = true;
            break;
          }
          if (wbuf[i] == 0x000D && i + 1 < wread && wbuf[i + 1] == 0x000A) {
            *dst++ = 0x000A;
            i++;
          } else {
            *dst++ = wbuf[i];
          }
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

        if ((unsigned)utf8.size() > remaining && f) {
          unsigned excess = (unsigned)utf8.size() - remaining;
          if (excess <= 4) {
            memcpy(f->u8_buf,
                   (const unsigned char *)utf8.data() + to_copy,
                   excess);
            f->u8_len = (int)excess;
          }
          break;
        }
      }

      return (int)total;
    }
  } // namespace musl
} // namespace mingw_thunk
