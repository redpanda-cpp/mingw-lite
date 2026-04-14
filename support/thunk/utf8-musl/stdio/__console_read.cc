#include "../internal/stdio_impl.h"
#include <io.h>
#include <thunk/string.h>
#include <wchar.h>

namespace mingw_thunk
{
  namespace musl
  {
    hidden size_t __console_read(FILE *f, unsigned char *buf, size_t len)
    {
      if (len == 0) {
        return 0;
      }

      HANDLE h = (HANDLE)_get_osfhandle(f->fd);
      if (h == INVALID_HANDLE_VALUE) {
        f->flags |= F_ERR;
        return 0;
      }

      size_t wbuf_size = 4096;
      wchar_t *wbuf = (wchar_t *)alloca(wbuf_size * sizeof(wchar_t));

      DWORD wread;
      if (!ReadConsoleW(h, wbuf, wbuf_size, &wread, nullptr)) {
        f->flags |= F_ERR;
        return 0;
      }

      if (wread == 0) {
        f->flags |= F_EOF;
        return 0;
      }

      wchar_t *src = wbuf;
      wchar_t *dst = wbuf;
      size_t actual_wread = 0;

      while (actual_wread < wread) {
        wchar_t c = src[actual_wread];

        if (c == 0x0004) {
          f->flags |= F_EOF;
          break;
        }

        if (c == 0x000D && actual_wread + 1 < wread &&
            src[actual_wread + 1] == 0x000A) {
          *dst++ = 0x000A;
          actual_wread += 2;
        } else {
          *dst++ = c;
          actual_wread++;
        }
      }

      size_t converted_wlen = dst - wbuf;

      d::u_str ubuf;
      if (!ubuf.from_w(wbuf, converted_wlen)) {
        f->flags |= F_ERR;
        return 0;
      }

      size_t total = ubuf.size();
      const unsigned char *udata = (const unsigned char *)ubuf.data();

      if (total == 0) {
        f->flags |= F_EOF;
        return 0;
      }

      if (f->buf_size > 0) {
        size_t caller_fit = len - 1;

        if (total <= caller_fit) {
          memcpy(buf, udata, total);
          return total;
        }

        memcpy(buf, udata, caller_fit);

        size_t file_len = total - caller_fit;
        if (file_len > f->buf_size)
          file_len = f->buf_size;
        memcpy(f->buf, udata + caller_fit, file_len);

        f->rpos = f->buf;
        f->rend = f->buf + file_len;

        buf[len - 1] = *f->rpos++;

        return len;
      } else {
        size_t copy_len = total;
        if (copy_len > len)
          copy_len = len;
        memcpy(buf, udata, copy_len);
        return copy_len;
      }
    }
  } // namespace musl
} // namespace mingw_thunk
