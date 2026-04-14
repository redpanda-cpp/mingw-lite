#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    int __overflow(FILE *f, int _c)
    {
      unsigned char c = _c;
      if (!f->wend && __towrite(f))
        return EOF;
      if (f->wpos != f->wend && c != f->lbf)
        return *f->wpos++ = c;
      if (f->write(f, &c, 1) != 1)
        return EOF;
      return c;
    }
  } // namespace musl
} // namespace mingw_thunk
