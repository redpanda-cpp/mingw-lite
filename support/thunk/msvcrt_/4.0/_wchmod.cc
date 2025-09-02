#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <io.h>
#include <wchar.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(int, _wchmod, const wchar_t *filename, int pmode)
  {
    if (internal::is_nt())
      return get__wchmod()(filename, pmode);

    stl::string a_name = internal::narrow(filename);
    return _chmod(a_name.c_str(), pmode);
  }
} // namespace mingw_thunk
