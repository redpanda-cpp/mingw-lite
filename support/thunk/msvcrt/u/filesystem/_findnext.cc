#include <thunk/_common.h>

#include <io.h>

#undef _findnext

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _findnext,
                 intptr_t handle,
#ifdef _WIN64
                 struct _finddata64i32_t *fileinfo
#else
                 struct _finddata32_t *fileinfo
#endif
  )
  {
#ifdef _WIN64
    return _findnext64i32(handle, fileinfo);
#else
    return _findnext32(handle, fileinfo);
#endif
  }
} // namespace mingw_thunk
