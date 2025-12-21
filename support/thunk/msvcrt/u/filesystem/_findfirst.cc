#include <thunk/_common.h>

#include <io.h>

#undef _findfirst

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 intptr_t,
                 __cdecl,
                 _findfirst,
                 const char *filespec,
#ifdef _WIN64
                 struct _finddata64i32_t *fileinfo
#else
                 struct _finddata32_t *fileinfo
#endif
  )
  {
#ifdef _WIN64
    return _findfirst64i32(filespec, fileinfo);
#else
    return _findfirst32(filespec, fileinfo);
#endif
  }
} // namespace mingw_thunk
