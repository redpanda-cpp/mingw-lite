#include "../internal/stdio_impl.h"
#include <io.h>
#include <thunk/string.h>
#include <thunk/unicode.h>
#include <wchar.h>

namespace mingw_thunk
{
  namespace musl
  {
    namespace
    {
      static size_t find_safe_chunk(const unsigned char *data, size_t max_len)
      {
        if (max_len == 0) {
          return 0;
        }

        size_t pos = 0;
        while (pos < max_len) {
          unsigned char lead = data[pos];
          int seq_len = i::u8_dec_len(lead);

          if (seq_len <= 0 || seq_len > 4) {
            pos++;
            continue;
          }

          if (pos + seq_len > max_len) {
            break;
          }

          bool valid = true;
          for (int i = 1; i < seq_len; ++i) {
            if (!i::u8_is_trail(data[pos + i])) {
              valid = false;
              break;
            }
          }

          if (!valid) {
            pos++;
            continue;
          }

          pos += seq_len;
        }

        return pos;
      }

      static size_t safe_chunk_size(const unsigned char *data, size_t remaining)
      {
        size_t max_chunk = remaining > 4096 ? 4096 : remaining;
        return find_safe_chunk(data, max_chunk);
      }

      static size_t write_utf8_data(HANDLE h,
                                    const unsigned char *data,
                                    size_t len,
                                    FILE *f,
                                    d::w_str &wbuf)
      {
        size_t total_written = 0;
        const unsigned char *ptr = data;
        size_t remaining = len;

        while (remaining > 0) {
          size_t chunk_size = safe_chunk_size(ptr, remaining);

          if (chunk_size == 0) {
            break;
          }

          if (!wbuf.from_u((const char *)ptr, (int)chunk_size)) {
            f->flags |= F_ERR;
            return total_written;
          }

          DWORD written;
          if (!WriteConsoleW(
                  h, wbuf.data(), (DWORD)wbuf.size(), &written, nullptr)) {
            f->flags |= F_ERR;
            return total_written;
          }

          if (written < (DWORD)wbuf.size()) {
            f->flags |= F_ERR;
            total_written += chunk_size;
            return total_written;
          }

          total_written += chunk_size;
          ptr += chunk_size;
          remaining -= chunk_size;
        }

        return total_written;
      }
    } // namespace

    hidden size_t __console_write(FILE *f, const unsigned char *buf, size_t len)
    {
      HANDLE h = (HANDLE)_get_osfhandle(f->fd);
      if (h == INVALID_HANDLE_VALUE) {
        f->flags |= F_ERR;
        return 0;
      }

      d::w_str wbuf;

      size_t pending_len = (size_t)f->u8_len;
      size_t buffered = f->wpos - f->wbase;

      if (len == 0 && pending_len == 0 && buffered == 0) {
        return 0;
      }

      size_t total = pending_len + buffered + len;
      unsigned char temp_buf[8192];
      unsigned char *merge_buf =
          total <= sizeof temp_buf ? temp_buf : (unsigned char *)alloca(total);

      unsigned char *p = merge_buf;
      if (pending_len > 0) {
        memcpy(p, f->u8_buf, pending_len);
        p += pending_len;
      }
      if (buffered > 0) {
        memcpy(p, f->wbase, buffered);
        p += buffered;
      }
      if (len > 0) {
        memcpy(p, buf, len);
        p += len;
      }

      size_t written = write_utf8_data(h, merge_buf, total, f, wbuf);
      size_t remaining = total - written;

      size_t consumed_from_buffered;
      if (written <= pending_len) {
        consumed_from_buffered = 0;
      } else {
        consumed_from_buffered = written - pending_len;
        if (consumed_from_buffered > buffered)
          consumed_from_buffered = buffered;
      }

      if (consumed_from_buffered >= buffered) {
        f->wbase = f->wpos = f->buf;
        f->wend = f->buf + f->buf_size;
      } else {
        f->wbase += consumed_from_buffered;
        f->wpos = f->wbase;
      }

      if (remaining > 0) {
        if (remaining <= 4) {
          memcpy(f->u8_buf, merge_buf + written, remaining);
          f->u8_len = (int)remaining;
        } else {
          f->u8_len = 0;
        }
      } else {
        f->u8_len = 0;
      }

      return len;
    }
  } // namespace musl
} // namespace mingw_thunk
