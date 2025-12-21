#include <thunk/_common.h>

#include <io.h>

#undef _findnexti64

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _findnexti64,
                 intptr_t handle,
#ifdef _WIN64
                 struct __finddata64_t *fileinfo
#else
                 struct _finddata32i64_t *fileinfo
#endif
  )
  {
#ifdef _WIN64
    return _findnext64(handle, fileinfo);
#else
    return _findnext32i64(handle, fileinfo);
#endif
  }
} // namespace mingw_thunk
