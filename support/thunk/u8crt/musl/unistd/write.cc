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
    namespace
    {
      size_t find_safe_chunk(const unsigned char *data, size_t max_len)
      {
        if (max_len == 0)
          return 0;

        size_t pos = 0;
        while (pos < max_len) {
          unsigned char lead = data[pos];
          int seq_len = i::u8_dec_len(lead);

          if (seq_len <= 0 || seq_len > 4) {
            pos++;
            continue;
          }

          if (pos + seq_len > max_len)
            break;

          bool valid = true;
          for (int j = 1; j < seq_len; ++j) {
            if (!i::u8_is_trail(data[pos + j])) {
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
    } // namespace

    int write(int fd, const void *buffer, unsigned int count)
    {
      HANDLE h = (HANDLE)_get_osfhandle(fd);
      if (h == INVALID_HANDLE_VALUE)
        return -1;

      FILE *f = g_fp_from_fd(fd);
      int plen = f ? f->u8_len : 0;
      size_t total = (size_t)plen + count;
      if (total == 0)
        return 0;

      unsigned char temp_buf[4096];
      unsigned char *merge =
          total <= sizeof temp_buf ? temp_buf : (unsigned char *)alloca(total);

      if (plen > 0) {
        memcpy(merge, f->u8_buf, plen);
        f->u8_len = 0;
      }
      if (count > 0)
        memcpy(merge + plen, buffer, count);

      size_t safe = find_safe_chunk(merge, total);
      if (safe == 0) {
        if (total <= 4 && f) {
          memcpy(f->u8_buf, merge, (size_t)total);
          f->u8_len = (int)total;
        }
        return count > 0 ? (int)count : 0;
      }

      d::w_str wbuf;
      if (!wbuf.from_u((const char *)merge, (int)safe))
        return -1;

      DWORD written;
      if (!WriteConsoleW(h, wbuf.data(), (DWORD)wbuf.size(), &written, nullptr))
        return -1;

      size_t tail = total - safe;
      if (tail > 0 && tail <= 4 && f) {
        memcpy(f->u8_buf, merge + safe, tail);
        f->u8_len = (int)tail;
      }

      return count > 0 ? (int)count : 0;
    }
  } // namespace musl
} // namespace mingw_thunk
