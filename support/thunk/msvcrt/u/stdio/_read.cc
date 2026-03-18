#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/u8crt/musl.h>

#include <io.h>

#include "@stdio.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, _read, int fd, void *buffer, unsigned int count)
  {
    if (!i::is_console(fd))
      return __ms__read(fd, buffer, count);

    return musl::read(fd, buffer, count);
  }
} // namespace mingw_thunk
