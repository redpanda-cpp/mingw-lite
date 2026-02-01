#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <sys/stat.h>

#undef _wstat32i64

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _wstat32i64,
                 const wchar_t *path,
                 struct _stat32i64 *buffer)
  {
    if (internal::is_nt())
      return __ms__wstat32i64(path, buffer);

    stl::string a_path = internal::w2a(path);
    return __ms__stat32i64(a_path.c_str(), buffer);
  }
} // namespace mingw_thunk
