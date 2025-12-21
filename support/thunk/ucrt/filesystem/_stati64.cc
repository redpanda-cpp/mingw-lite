#include <thunk/_common.h>
#include <thunk/string.h>

#include <sys/stat.h>
#include <sys/types.h>

#undef _stati64

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
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
