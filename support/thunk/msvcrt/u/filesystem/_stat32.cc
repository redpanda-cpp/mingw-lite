#include <thunk/_common.h>
#include <thunk/string.h>

#include "_stat.h"

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _stat32,
                 const char *path,
                 struct _stat32 *buffer)
  {
    stl::wstring w_path = internal::u2w(path);
    return _wstat32(w_path.c_str(), buffer);
  }

  __DECLARE_FORCE_OVERRIDE_MINGW_EMU(_stat32)
} // namespace mingw_thunk
