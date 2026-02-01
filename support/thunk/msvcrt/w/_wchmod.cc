#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <io.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, _wchmod, const wchar_t *filename, int pmode)
  {
    if (internal::is_nt())
      return __ms__wchmod(filename, pmode);

    stl::string a_name = internal::w2a(filename);
    return __ms__chmod(a_name.c_str(), pmode);
  }
} // namespace mingw_thunk
