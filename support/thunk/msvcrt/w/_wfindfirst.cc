#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <io.h>

#include <windows.h>

#undef _wfindfirst

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 intptr_t,
                 __cdecl,
                 _wfindfirst,
                 const wchar_t *filespec,
                 struct _wfinddata32_t *fileinfo)
  {
    if (internal::is_nt())
      return __ms__wfindfirst32(filespec, fileinfo);

    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(filespec, &fd);
    if (h == INVALID_HANDLE_VALUE) {
      internal::dosmaperr(GetLastError());
      return -1;
    }

    fileinfo->attrib = fd.dwFileAttributes;
    fileinfo->time_create = internal::c_time64_from_filetime(fd.ftCreationTime);
    fileinfo->time_access =
        internal::c_time64_from_filetime(fd.ftLastAccessTime);
    fileinfo->time_write = internal::c_time64_from_filetime(fd.ftLastWriteTime);
    fileinfo->size = fd.nFileSizeLow;
    wcsncpy(fileinfo->name, fd.cFileName, MAX_PATH);

    return reinterpret_cast<intptr_t>(h);
  }
} // namespace mingw_thunk
