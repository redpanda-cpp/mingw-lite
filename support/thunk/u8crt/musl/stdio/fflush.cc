#include "../internal/stdio_impl.h"

namespace mingw_thunk
{
  namespace musl
  {
    int fflush(FILE *f)
    {
      if (!f) {
        /* ignore */
      }

      FLOCK(f);

      /* If writing, flush output */
      if (f->wpos != f->wbase) {
        f->write(f, 0, 0);
        if (!f->wpos) {
          FUNLOCK(f);
          return EOF;
        }
      }

      /* If reading, sync position, per POSIX */
      if (f->rpos != f->rend)
        f->seek(f, f->rpos - f->rend, SEEK_CUR);

      /* Clear read and write modes */
      f->wpos = f->wbase = f->wend = 0;
      f->rpos = f->rend = 0;

      FUNLOCK(f);
      return 0;
    }
  } // namespace musl
} // namespace mingw_thunk
