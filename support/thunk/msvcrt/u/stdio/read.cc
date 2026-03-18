#include <thunk/_common.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, read, int fd, void *buffer, unsigned int count)
  {
    return _read(fd, buffer, count);
  }
} // namespace mingw_thunk
