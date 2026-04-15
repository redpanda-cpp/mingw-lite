#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/wntcrt/stat.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _wstat32i64,
                 const wchar_t *path,
                 struct _stat32i64 *buffer)
  {
    return __ms__wstat32i64(path, buffer);
  }
} // namespace mingw_thunk
