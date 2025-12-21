#include <thunk/_common.h>

#include <io.h>

#undef _findfirsti64

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
                 0,
                 intptr_t,
                 __cdecl,
                 _findfirsti64,
                 const char *filespec,
#ifdef _WIN64
                 struct __finddata64_t *fileinfo
#else
                 struct _finddata32i64_t *fileinfo
#endif
  )
  {
#ifdef _WIN64
    return _findfirst64(filespec, fileinfo);
#else
    return _findfirst32i64(filespec, fileinfo);
#endif
  }
} // namespace mingw_thunk
