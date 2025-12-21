#include <thunk/_common.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, unlink, const char *path)
  {
    return _unlink(path);
  }
} // namespace mingw_thunk
