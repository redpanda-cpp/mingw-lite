#include "_wutime64.h"

#include <thunk/_common.h>
#include <thunk/os.h>

#include <fcntl.h>
#include <sys/utime.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _wutime64,
                 const wchar_t *filename,
                 struct __utimbuf64 *times)
  {
    if (const auto pfn = try_get__wutime64(); pfn && internal::is_nt())
      return pfn(filename, times);

    return impl::time32__wutime64(filename, times);
  }

  namespace impl
  {
    int time32__wutime64(const wchar_t *filename, struct __utimbuf64 *times)
    {
      int fd = _wopen(filename, _O_BINARY | _O_WRONLY);
      if (fd < 0)
        return -1;

      int ret = _futime64(fd, times);
      _close(fd);
      return ret;
    }
  } // namespace impl
} // namespace mingw_thunk
