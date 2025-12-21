#include <thunk/_common.h>

#include <sys/stat.h>
#include <sys/types.h>

#undef stat64

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 stat64,
                 const char *path,
                 struct stat64 *buffer)
  {
    return _stat64(path, (struct _stat64 *)buffer);
  }
} // namespace mingw_thunk
