#define __CRT__NO_INLINE

#include <thunk/_common.h>
#include <thunk/crt_find_data.h>
#include <thunk/string.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(api_ms_win_crt_filesystem_l1_1_0,
                 0,
                 intptr_t,
                 __cdecl,
                 _findfirst64i32,
                 const char *filespec,
                 struct _finddata64i32_t *fileinfo)
  {
    _wfinddata64i32_t w_file_info;

    stl::wstring w_file_spec = internal::u2w(filespec);
    intptr_t res = _wfindfirst64i32(w_file_spec.c_str(), &w_file_info);

    if (res == intptr_t(-1))
      return res;

    *fileinfo = internal::w2u(w_file_info);
    return res;
  }
} // namespace mingw_thunk
