#include <thunk/_common.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <io.h>
#include <sys/utime.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(int, _futime64, int fd, struct __utimbuf64 *filetime)
  {
    if (const auto pfn = try_get__futime64())
      return pfn(fd, filetime);

    HANDLE hFile = reinterpret_cast<HANDLE>(_get_osfhandle(fd));

    FILETIME atime = internal::filetime_from_c_time64(filetime->actime);
    FILETIME mtime = internal::filetime_from_c_time64(filetime->modtime);

    if (!SetFileTime(hFile, nullptr, &atime, &mtime)) {
      internal::dosmaperr(GetLastError());
      return -1;
    }

    return 0;
  };
} // namespace mingw_thunk
