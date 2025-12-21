#include <thunk/_common.h>

#include <stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt, 0, int, __cdecl, putenv, const char *string)
  {
    return _putenv(string);
  }
} // namespace mingw_thunk
