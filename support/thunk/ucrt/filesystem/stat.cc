#include <thunk/_common.h>

#include <sys/stat.h>
#include <sys/types.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 stat,
                 const char *path,
                 struct stat *buffer)
  {
#ifdef _WIN64
    return _stat32(path, (struct _stat32 *)buffer);
#else
    return _stat64i32(path, (struct _stat64i32 *)buffer);
#endif
  }
} // namespace mingw_thunk
