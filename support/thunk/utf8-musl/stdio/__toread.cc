#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    int __toread(FILE *f) noexcept
    {
      f->mode |= f->mode - 1;
      if (f->wpos != f->wbase)
        f->write(f, 0, 0);
      f->wpos = f->wbase = f->wend = 0;
      if (f->flags & F_NORD) {
        f->flags |= F_ERR;
        return EOF;
      }
      f->rpos = f->rend = f->buf + f->buf_size;
      return (f->flags & F_EOF) ? EOF : 0;
    }
  } // namespace musl
} // namespace mingw_thunk
