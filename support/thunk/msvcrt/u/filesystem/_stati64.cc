#include <thunk/_common.h>
#include <thunk/string.h>

#include "_stat.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _stati64,
                 const char *path,
#ifdef _WIN64
                 struct _stat64 *buffer
#else
                 struct _stat32i64 *buffer
#endif
  )
  {
#ifdef _WIN64
    return _stat64(path, buffer);
#else
    return _stat32i64(path, buffer);
#endif
  }
} // namespace mingw_thunk
