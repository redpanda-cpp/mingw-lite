#pragma once

#include "../include/features.h"

#include <stdio.h>

#include <windows.h>

namespace mingw_thunk
{
  namespace musl
  {
    constexpr int UNGET = 8;

    enum
    {
      F_PERM = 1,
      F_NORD = 4,
      F_NOWR = 8,
      F_EOF = 16,
      F_ERR = 32,
      F_SVB = 64,
      F_APP = 128
    };

    struct FILE
    {
      unsigned flags;
      unsigned char *rpos, *rend;
      int (*close)(FILE *);
      unsigned char *wend, *wpos;
      unsigned char *mustbezero_1;
      unsigned char *wbase;
      size_t (*read)(FILE *, unsigned char *, size_t);
      size_t (*write)(FILE *, const unsigned char *, size_t);
      off_t (*seek)(FILE *, off_t, int);
      unsigned char *buf;
      size_t buf_size;
      FILE *prev, *next;
      int fd;
      int pipe_pid;
      long lockcount;
      int mode;
      volatile int lock;
      int lbf;
      void *cookie;
      off_t off;
      char *getln_buf;
      void *mustbezero_2;
      unsigned char *shend;
      off_t shlim, shcnt;
      FILE *prev_locked, *next_locked;
      struct __locale_struct *locale;

      // u8crt addition
      unsigned char u8_buf[4];
      int u8_len;
    };

    inline void FLOCK(FILE *f) noexcept
    {
      while (!__atomic_test_and_set(&f->lock, __ATOMIC_ACQUIRE))
        Sleep(0);
    }

    inline void FUNLOCK(FILE *f) noexcept
    {
      __atomic_clear(&f->lock, __ATOMIC_RELEASE);
    }

    hidden size_t __console_read(FILE *, unsigned char *, size_t);
    hidden size_t __console_write(FILE *, const unsigned char *, size_t);
    hidden off_t __console_seek(FILE *, off_t, int);
    hidden int __console_close(FILE *);

    hidden int __toread(FILE *);
    hidden int __towrite(FILE *);

    hidden int __overflow(FILE *, int);
    hidden int __uflow(FILE *);

    hidden size_t __fwritex(const unsigned char *, size_t, FILE *);

    inline int feof(FILE *f) noexcept
    {
      return f->flags & F_EOF;
    }

    inline int ferror(FILE *f) noexcept
    {
      return f->flags & F_ERR;
    }

    inline int getc_unlocked(FILE *f) noexcept
    {
      return f->rpos != f->rend ? *f->rpos++ : __uflow(f);
    }

    inline int putc_unlocked(int c, FILE *f) noexcept
    {
      return (unsigned char)c != f->lbf && f->wpos != f->wend
                 ? *f->wpos++ = (unsigned char)c
                 : __overflow(f, (unsigned char)c);
    }
  } // namespace musl
} // namespace mingw_thunk
