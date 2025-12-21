#pragma once

#include <thunk/string.h>
#include <thunk/unicode.h>

#include <io.h>
#include <stdio.h>

#include <nostl/map.h>
#include <nostl/string.h>

#include <windows.h>

namespace mingw_thunk
{
  namespace internal
  {
    inline bool is_console(HANDLE fh) noexcept
    {
      DWORD mode;
      return GetConsoleMode(fh, &mode);
    }

    inline bool is_console(int fd) noexcept
    {
      return (fd == 0 || fd == 1 || fd == 2) && _isatty(fd) &&
             is_console((HANDLE)_get_osfhandle(fd));
    }

    inline bool is_console(FILE *fp) noexcept
    {
      return is_console(_fileno(fp));
    }

    inline bool is_buffered(int fd) noexcept
    {
      // there's no public API to get buffer status
      // we assume only stderr is unbuffered
      return fd != 2;
    }

    inline bool is_buffered(FILE *fp) noexcept
    {
      return is_buffered(_fileno(fp));
    }

    struct io_buffer
    {
    private:
      bool lock;
      stl::string buffer;

    public:
      io_buffer() : lock(false)
      {
      }

    public:
      void append(const char *data, size_t size)
      {
        acquire();
        buffer.append(data, size);
        release();
      }

      void discard()
      {
        acquire();
        buffer.clear();
        release();
      }

      void flush_all(int fd)
      {
        flush_range(fd, buffer.size());
      }

      void flush_complete_line(int fd)
      {
        acquire();
        size_t pos = buffer.rfind('\n');
        if (pos != stl::string::npos)
          flush_range_no_lock(fd, pos + 1);
        release();
      }

      void flush_complete_sequence(int fd)
      {
        acquire();
        size_t next_start = 0;
        while (next_start < buffer.size()) {
          int seq_len = u8_dec_len(buffer[next_start]);
          if (seq_len <= 1) {
            // invalid or ASCII
            next_start += 1;
          } else {
            if (next_start + seq_len <= buffer.size()) {
              bool all_trails = true;
              for (int i = 1; i < seq_len; ++i) {
                if (!u8_is_trail(buffer[next_start + i])) {
                  all_trails = false;
                  break;
                }
              }
              if (all_trails) {
                // good, go on
                next_start += seq_len;
              } else {
                // oops broken
                // +2/+3 is unneccesary (will be skipped on next iteration)
                next_start += 1;
              }
            } else {
              // waiting for further bytes
              break;
            }
          }
        }
        flush_range_no_lock(fd, next_start);
        release();
      }

      void flush_range(int fd, size_t end)
      {
        acquire();
        flush_range_no_lock(fd, end);
        release();
      }

      void push(char ch)
      {
        acquire();
        buffer.push_back(ch);
        release();
      }

    private:
      void flush_range_no_lock(HANDLE console, size_t end)
      {
        stl::wstring w_buffer = u2w(buffer.data(), end);
        DWORD w_written;
        WriteConsoleW(
            console, w_buffer.data(), w_buffer.size(), &w_written, nullptr);
        if (end < buffer.size()) {
          libc::memmove(
              buffer.data(), buffer.data() + end, buffer.size() - end);
          buffer.resize(buffer.size() - end);
        } else
          buffer.clear();
      }

      void flush_range_no_lock(int fd, size_t end)
      {
        HANDLE h = (HANDLE)_get_osfhandle(fd);
        flush_range_no_lock(h, end);
      }

    private:
      void acquire()
      {
        while (__atomic_test_and_set(&lock, __ATOMIC_ACQUIRE))
          Sleep(0);
      }

      void release()
      {
        __atomic_clear(&lock, __ATOMIC_RELEASE);
      }
    };

    extern io_buffer stdio_buffer[3];
  } // namespace internal
} // namespace mingw_thunk
