#include <thunk/_common.h>

#include <fcntl.h>
#include <sys/utime.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(int,
                     _wutime64,
                     const wchar_t *filename,
                     struct __utimbuf64 *times)
  {
    if (const auto pfn = try_get__wutime64())
      return pfn(filename, times);

    int fd = _wopen(filename, _O_BINARY | _O_WRONLY);
    if (fd < 0)
      return -1;

    int ret = _futime64(fd, times);
    _close(fd);
    return ret;
  }
} // namespace mingw_thunk
