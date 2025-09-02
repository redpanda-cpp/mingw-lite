#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <sys/stat.h>
#include <wchar.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(int, _wstat32, const wchar_t *path, struct _stat32 *buffer)
  {
    if (internal::is_nt()) {
      static auto *pfn =
          internal::module_msvcrt.get_function<fn__wstat32_t>("_wstat");
      return pfn(path, buffer);
    }

    stl::string a_path = internal::narrow(path);
    return _stat32(a_path.c_str(), buffer);
  }
} // namespace mingw_thunk
