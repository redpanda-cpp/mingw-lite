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
                 _stat64,
                 const char *path,
                 struct _stat64 *buffer)
  {
    stl::wstring w_path = internal::u2w(path);
    return _wstat64(w_path.c_str(), buffer);
  }
} // namespace mingw_thunk
