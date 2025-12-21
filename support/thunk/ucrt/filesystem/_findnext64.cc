#include <thunk/_common.h>
#include <thunk/crt_find_data.h>
#include <thunk/string.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
                 0,
                 int,
                 __cdecl,
                 _findnext64,
                 intptr_t handle,
                 struct __finddata64_t *fileinfo)
  {
    _wfinddata64_t w_file_info;
    int res = _wfindnext64(handle, &w_file_info);

    if (res == -1)
      return res;

    *fileinfo = internal::w2u(w_file_info);
    return res;
  }
} // namespace mingw_thunk
