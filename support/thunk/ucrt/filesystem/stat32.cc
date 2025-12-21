#include <thunk/_common.h>

#include <sys/stat.h>
#include <sys/types.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 stat32,
                 const char *path,
                 struct _stat32 *buffer)
  {
    return _stat32(path, buffer);
  }
} // namespace mingw_thunk
