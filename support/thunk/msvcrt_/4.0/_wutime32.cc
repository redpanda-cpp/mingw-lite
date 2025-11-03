#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <sys/utime.h>
#include <wchar.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(int,
                     _wutime32,
                     const wchar_t *filename,
                     struct __utimbuf32 *times)
  {
    if (internal::is_nt()) {
      static auto *pfn =
          internal::module_msvcrt.get_function<fn__wutime32_t>("_wutime");
      return pfn(filename, times);
    }

    stl::string a_path = internal::narrow(filename);
    return _utime32(a_path.c_str(), times);
  }
} // namespace mingw_thunk
