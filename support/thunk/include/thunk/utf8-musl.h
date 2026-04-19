#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>

namespace mingw_thunk
{
  namespace musl
  {
    struct FILE;
    struct iovec;

    extern struct FILE *g_stdin;
    extern struct FILE *g_stdout;
    extern struct FILE *g_stderr;

    inline FILE *g_fp_from_fd(int fd)
    {
      switch (fd) {
      case 0:
        return g_stdin;
      case 1:
        return g_stdout;
      case 2:
        return g_stderr;
      default:
        return nullptr;
      }
    }

    int fflush(FILE *f);
    int fgetc(FILE *f);
    char *fgets(char *s, int n, FILE *f);
    int fputc(int c, FILE *f);
    size_t fread(void *dest, size_t size, size_t nmemb, FILE *f);
    size_t fwrite(const void *src, size_t size, size_t nmemb, FILE *f);
    int ungetc(int c, FILE *f);
    int vfprintf(FILE *f, const char *fmt, va_list ap);
    int vfscanf(FILE *f, const char *fmt, va_list ap);

    ssize_t read(int fd, void *buf, size_t count);
    ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
    ssize_t write(int fd, const void *buf, size_t count);
    ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
  } // namespace musl
} // namespace mingw_thunk
