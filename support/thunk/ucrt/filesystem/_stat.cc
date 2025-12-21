#include <thunk/_common.h>
#include <thunk/string.h>

#include <sys/stat.h>
#include <sys/types.h>

#undef _stat

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
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
