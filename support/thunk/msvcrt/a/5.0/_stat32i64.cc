#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/wntcrt/stat.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _stat32i64,
                 const char *path,
                 struct _stat32i64 *buffer)
  {
    return __ms__stat32i64(path, (struct _stati64 *)buffer);
  }
} // namespace mingw_thunk
