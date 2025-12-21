#include <thunk/_common.h>
#include <thunk/string.h>

#include <sys/stat.h>
#include <sys/types.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 _stat32i64,
                 const char *path,
                 struct _stat32i64 *buffer)
  {
    stl::wstring w_path = internal::u2w(path);
    return _wstat32i64(w_path.c_str(), buffer);
  }
} // namespace mingw_thunk
