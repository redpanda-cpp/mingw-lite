#include "_utime64.h"

#include <thunk/_common.h>

#include <fcntl.h>
#include <sys/utime.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _utime64,
                 const char *filename,
                 struct __utimbuf64 *times)
  {
    if (const auto pfn = try_get__utime64())
      return pfn(filename, times);

    return impl::time32__utime64(filename, times);
  }

  namespace impl
  {
    int time32__utime64(const char *filename, struct __utimbuf64 *times)
    {
      int fd = _open(filename, _O_BINARY | _O_WRONLY);
      if (fd < 0)
        return -1;

      int ret = _futime64(fd, times);
      _close(fd);
      return ret;
    }
  } // namespace impl
} // namespace mingw_thunk
