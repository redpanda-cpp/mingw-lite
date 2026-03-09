#include <thunk/_common.h>

#include <thunk/wntcrt/stat.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, fstat64, int fd, struct stat64 *buffer)
  {
    return _fstat64(fd, (struct _stat64 *)buffer);
  }
} // namespace mingw_thunk
