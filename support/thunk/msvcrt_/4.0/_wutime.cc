#define __CRT__NO_INLINE
#define _USE_32BIT_TIME_T

#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <sys/utime.h>
#include <wchar.h>

namespace mingw_thunk
{
  static_assert(sizeof(_utimbuf) == sizeof(__utimbuf32));

  __DEFINE_CRT_THUNK(int,
                     _wutime,
                     const wchar_t *filename,
                     struct _utimbuf *times)
  {
    if (internal::is_nt())
      return get__wutime()(filename, times);

    stl::string a_path = internal::narrow(filename);
    return _utime32(a_path.c_str(), (__utimbuf32 *)times);
  }
} // namespace mingw_thunk
