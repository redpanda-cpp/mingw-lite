#include <thunk/_common.h>
#include <thunk/crt_find_data.h>
#include <thunk/string.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _findnext32,
                 intptr_t handle,
                 struct _finddata32_t *fileinfo)
  {
    _wfinddata32_t w_file_info;
    int res = _wfindnext32(handle, &w_file_info);

    if (res == -1)
      return res;

    *fileinfo = internal::w2u(w_file_info);
    return res;
  }

  // upstream added this for msvcrt64
  __DECLARE_FORCE_OVERRIDE_MINGW_EMU(_findnext32)
} // namespace mingw_thunk
