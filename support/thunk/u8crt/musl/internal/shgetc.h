#pragma once

#include "stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    hidden void __shlim(FILE *, off_t);
    hidden int __shgetc(FILE *);

    inline off_t shcnt(FILE *f) noexcept
    {
      return f->shcnt + (f->rpos - f->buf);
    }

    inline void shlim(FILE *f, off_t lim) noexcept
    {
      __shlim(f, lim);
    }

    inline int shgetc(FILE *f) noexcept
    {
      return (f->rpos != f->shend) ? *f->rpos++ : __shgetc(f);
    }

    inline void shunget(FILE *f) noexcept
    {
      if (f->shlim >= 0)
        f->rpos--;
    }
  } // namespace musl
} // namespace mingw_thunk
