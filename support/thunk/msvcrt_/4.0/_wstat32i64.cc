#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <sys/stat.h>
#include <wchar.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(int,
                     _wstat32i64,
                     const wchar_t *path,
                     struct _stat32i64 *buffer)
  {
    if (internal::is_nt()) {
      static auto *pfn =
          internal::module_msvcrt.get_function<fn__wstat32i64_t>("_wstati64");
      return pfn(path, buffer);
    }

    stl::string a_path = internal::narrow(path);
    return _stat32i64(a_path.c_str(), buffer);
  }
} // namespace mingw_thunk
