#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/u8crt/musl.h>

#include <stdio.h>

#include "@stdio.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 size_t,
                 __cdecl,
                 fread,
                 void *ptr,
                 size_t size,
                 size_t count,
                 FILE *stream)
  {
    int fd = _fileno(stream);
    if (!i::is_console(fd))
      return __ms_fread(ptr, size, count, stream);

    return musl::fread(ptr, size, count, musl::g_fp_from_fd(fd));
  }
} // namespace mingw_thunk
