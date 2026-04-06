#include <thunk/_common.h>
#include <thunk/crt_find_data.h>
#include <thunk/string.h>

#include <errno.h>
#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 intptr_t,
                 __cdecl,
                 _findfirst64i32,
                 const char *filespec,
                 struct _finddata64i32_t *fileinfo)
  {
    if (!filespec || !fileinfo) {
      _set_errno(EINVAL);
      return -1;
    }

    _wfinddata64i32_t w_file_info;

    d::w_str w_file_spec;
    if (!w_file_spec.from_u(filespec)) {
      _set_errno(ENOMEM);
      return -1;
    }

    intptr_t res = _wfindfirst64i32(w_file_spec.c_str(), &w_file_info);

    if (res == intptr_t(-1))
      return res;

    *fileinfo = internal::w2u(w_file_info);
    return res;
  }

  // upstream added this for old msvcrt32
  __DECLARE_FORCE_OVERRIDE_MINGW_EMU(_findfirst64i32)
} // namespace mingw_thunk
