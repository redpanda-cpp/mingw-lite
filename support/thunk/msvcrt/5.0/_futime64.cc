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
    __DISPATCH_THUNK_2(_futime64,
                       const auto pfn = try_get__futime64(),
                       pfn,
                       &f::time32__futime64);

    return dllimport__futime64(fd, filetime);
  };

  namespace f
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
  } // namespace f
} // namespace mingw_thunk
