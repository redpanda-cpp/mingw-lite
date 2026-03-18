#include <thunk/_common.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 write,
                 int fd,
                 const void *buffer,
                 unsigned int count)
  {
    return _write(fd, buffer, count);
  }
} // namespace mingw_thunk
