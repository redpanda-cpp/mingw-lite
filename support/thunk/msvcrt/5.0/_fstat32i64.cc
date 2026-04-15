#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/wntcrt/stat.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, _fstat32i64, int fd, struct _stat32i64 *buffer)
  {
    return __ms__fstat32i64(fd, buffer);
  }
} // namespace mingw_thunk
