#include <thunk/_common.h>
#include <thunk/wntcrt/stat.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _stat,
                 const char *path,
#ifdef _WIN64
                 struct _stat64i32 *buffer
#else
                 struct _stat32 *buffer
#endif
  )
  {
#ifdef _WIN64
    return _stat64i32(path, buffer);
#else
    return _stat32(path, buffer);
#endif
  }
} // namespace mingw_thunk
