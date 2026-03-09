#include "_stat64.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/stat.h>
#include <thunk/wntcrt/time.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _stat64,
                 const char *path,
                 struct _stat64 *buffer)
  {
    __DISPATCH_THUNK_2(
        _stat64, const auto pfn = try_get__stat64(), pfn, &f::time32__stat64);

    return dllimport__stat64(path, buffer);
  }

  namespace f
  {
    int time32__stat64(const char *path, struct __stat64 *buffer)
    {
      struct _stat32i64 buffer32;
      int ret = _stat32i64(path, &buffer32);
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

      WIN32_FILE_ATTRIBUTE_DATA attr;
      if (!GetFileAttributesExA(path, GetFileExInfoStandard, &attr)) {
        internal::dosmaperr(GetLastError());
        return -1;
      }

      buffer->st_atime =
          internal::c_time64_from_filetime(attr.ftLastAccessTime);
      buffer->st_mtime = internal::c_time64_from_filetime(attr.ftLastWriteTime);
      buffer->st_ctime = internal::c_time64_from_filetime(attr.ftCreationTime);

      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
