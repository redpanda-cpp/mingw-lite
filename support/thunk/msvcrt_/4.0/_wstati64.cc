#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <sys/stat.h>
#include <wchar.h>

#undef _wstati64

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(int,
                     _wstati64,
                     const wchar_t *path,
                     struct _stat32i64 *buffer)
  {
    if (internal::is_nt())
      return get__wstati64()(path, buffer);

    stl::string a_path = internal::narrow(path);
    return _stat32i64(a_path.c_str(), buffer);
  }
} // namespace mingw_thunk
