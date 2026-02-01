#include "_fstat64.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <io.h>
#include <sys/stat.h>

#include <windows.h>

namespace mingw_thunk
{

  __DEFINE_THUNK(
      msvcrt, 0, int, __cdecl, _fstat64, int fd, struct _stat64 *buffer)
  {
    if (const auto pfn = try_get__fstat64())
      return pfn(fd, buffer);

    return impl::time32__fstat64(fd, buffer);
  }

  namespace impl
  {
    int time32__fstat64(int fd, struct _stat64 *buffer)
    {
      struct _stat32i64 buffer32;
      int ret = _fstat32i64(fd, &buffer32);
      if (ret != 0)
        return ret;

      buffer->st_dev = buffer32.st_dev;
      buffer->st_ino = buffer32.st_ino;
      buffer->st_mode = buffer32.st_mode;
      buffer->st_nlink = buffer32.st_nlink;
      buffer->st_uid = buffer32.st_uid;
      buffer->st_gid = buffer32.st_gid;
      buffer->st_rdev = buffer32.st_rdev;
      buffer->st_size = buffer32.st_size;

      HANDLE hFile = reinterpret_cast<HANDLE>(_get_osfhandle(fd));

      FILETIME atime, mtime, ctime;

      if (!GetFileTime(hFile, &ctime, &atime, &mtime)) {
        internal::dosmaperr(GetLastError());
        return -1;
      }

      buffer->st_atime = internal::c_time64_from_filetime(atime);
      buffer->st_mtime = internal::c_time64_from_filetime(mtime);
      buffer->st_ctime = internal::c_time64_from_filetime(ctime);

      return 0;
    }
  } // namespace impl
} // namespace mingw_thunk
