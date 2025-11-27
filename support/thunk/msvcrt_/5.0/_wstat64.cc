#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <sys/stat.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(int, _wstat64, const wchar_t *path, struct _stat64 *buffer)
  {
    if (const auto pfn = try_get__wstat64(); pfn && internal::is_nt())
      return pfn(path, buffer);

    struct _stat32i64 buffer32;
    int ret = _wstat32i64(path, &buffer32);
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
    if (!GetFileAttributesExW(path, GetFileExInfoStandard, &attr)) {
      internal::dosmaperr(GetLastError());
      return -1;
    }

    buffer->st_atime = internal::c_time64_from_filetime(attr.ftLastAccessTime);
    buffer->st_mtime = internal::c_time64_from_filetime(attr.ftLastWriteTime);
    buffer->st_ctime = internal::c_time64_from_filetime(attr.ftCreationTime);

    return 0;
  }
} // namespace mingw_thunk
