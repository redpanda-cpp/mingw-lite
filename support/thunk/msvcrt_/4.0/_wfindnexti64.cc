#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <io.h>

#include <windows.h>

#undef _wfindnexti64

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(int,
                     _wfindnexti64,
                     intptr_t handle,
                     struct _wfinddatai64_t *fileinfo)
  {
    if (internal::is_nt())
      return get__wfindnexti64()(handle, fileinfo);

    HANDLE h = reinterpret_cast<HANDLE>(handle);

    WIN32_FIND_DATAW fd;
    if (!FindNextFileW(h, &fd)) {
      internal::dosmaperr(GetLastError());
      return -1;
    }

    ULARGE_INTEGER size = {
        .LowPart = fd.nFileSizeLow,
        .HighPart = fd.nFileSizeHigh,
    };

    fileinfo->attrib = fd.dwFileAttributes;
    fileinfo->time_create = internal::c_time64_from_filetime(fd.ftCreationTime);
    fileinfo->time_access =
        internal::c_time64_from_filetime(fd.ftLastAccessTime);
    fileinfo->time_write = internal::c_time64_from_filetime(fd.ftLastWriteTime);
    fileinfo->size = size.QuadPart;
    wcsncpy(fileinfo->name, fd.cFileName, MAX_PATH);

    return 0;
  }
} // namespace mingw_thunk
