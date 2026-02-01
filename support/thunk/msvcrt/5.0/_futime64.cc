#include "_futime64.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <io.h>
#include <sys/utime.h>

#include <windows.h>

namespace mingw_thunk
{

  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, _futime64, int fd, struct __utimbuf64 *filetime)
  {
    if (const auto pfn = try_get__futime64())
      return pfn(fd, filetime);

    return impl::time32__futime64(fd, filetime);
  };

  namespace impl
  {
    int time32__futime64(int fd, struct __utimbuf64 *filetime)
    {
      HANDLE hFile = reinterpret_cast<HANDLE>(_get_osfhandle(fd));

      FILETIME atime = internal::filetime_from_c_time64(filetime->actime);
      FILETIME mtime = internal::filetime_from_c_time64(filetime->modtime);

      if (!SetFileTime(hFile, nullptr, &atime, &mtime)) {
        internal::dosmaperr(GetLastError());
        return -1;
      }

      return 0;
    }
  } // namespace impl
} // namespace mingw_thunk
