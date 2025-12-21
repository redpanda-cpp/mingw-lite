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
                 _findfirst64,
                 const char *filespec,
                 struct __finddata64_t *fileinfo)
  {
    _wfinddata64_t w_file_info;

    stl::wstring w_file_spec = internal::u2w(filespec);
    intptr_t res = _wfindfirst64(w_file_spec.c_str(), &w_file_info);

    if (res == intptr_t(-1))
      return res;

    *fileinfo = internal::w2u(w_file_info);
    return res;
  }
} // namespace mingw_thunk
