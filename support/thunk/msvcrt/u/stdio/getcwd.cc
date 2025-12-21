#include <thunk/_common.h>

#include <direct.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, char *, __cdecl, getcwd, char *buf, int size)
  {
    return _getcwd(buf, size);
  }
} // namespace mingw_thunk
