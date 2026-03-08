#pragma once

#include <thunk/buffer.h>
#include <thunk/string.h>
#include <thunk/unicode.h>

#include <io.h>
#include <stdio.h>

#include <windows.h>

namespace mingw_thunk
{
  namespace i
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
  } // namespace i

  namespace d
  {
    struct io_buffer
    {
    private:
      bool lock;
      d::buffer<1280, char> buffer;

    private:
      static constexpr size_t buffer_flush_threshold = 1024;

    public:
      io_buffer() : lock(false)
      {
      }

    public:
      size_t size() const noexcept
      {
        return buffer.size();
      }

      char *data() noexcept
      {
        return buffer.data();
      }

      const char *data() const noexcept
      {
        return buffer.data();
      }

      size_t rfind(char ch) const noexcept
      {
        for (size_t i = buffer.size(); i > 0; --i) {
          if (buffer[i - 1] == ch)
            return i - 1;
        }
        return size_t(-1);
      }

      bool resize(size_t new_size) noexcept
      {
        acquire();
        return buffer.resize(new_size);
        release();
      }

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

      void flush_if_reaching_threshold(int fd)
      {
        if (buffer.size() >= buffer_flush_threshold)
          flush_complete_sequence(fd);
      }

      void flush_complete_line(int fd)
      {
        acquire();
        size_t pos = rfind('\n');
        if (pos != size_t(-1))
          flush_range_no_lock(fd, pos + 1);
        release();
      }

      void flush_complete_sequence(int fd)
      {
        acquire();
        size_t next_start = 0;
        while (next_start < buffer.size()) {
          int seq_len = i::u8_dec_len(buffer[next_start]);
          if (seq_len <= 1) {
            // invalid or ASCII
            next_start += 1;
          } else {
            if (next_start + seq_len <= buffer.size()) {
              bool all_trails = true;
              for (int i = 1; i < seq_len; ++i) {
                if (!i::u8_is_trail(buffer[next_start + i])) {
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
        d::w_str w_buffer;
        if (w_buffer.from_u(buffer.data(), end)) {
          DWORD w_written;
          WriteConsoleW(
              console, w_buffer.data(), w_buffer.size(), &w_written, nullptr);
        }
        if (end < buffer.size()) {
          memmove(buffer.data(), buffer.data() + end, buffer.size() - end);
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
  } // namespace d

  namespace g
  {
    extern d::io_buffer stdio_buffer[3];
  }
} // namespace mingw_thunk
