#include "_wfindnext32i64.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <io.h>

#include <windows.h>

#undef _wfindnext32i64

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _wfindnext32i64,
                 intptr_t handle,
                 struct _wfinddata32i64_t *fileinfo)
  {
    __DISPATCH_THUNK_2(_wfindnext32i64,
                       i::is_nt(),
                       &__ms__wfindnext32i64,
                       &f::win9x__wfindnext32i64);

    return dllimport__wfindnext32i64(handle, fileinfo);
  }

  namespace f
  {
    int win9x__wfindnext32i64(intptr_t handle,
                              struct _wfinddata32i64_t *fileinfo)
    {
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
      fileinfo->time_create =
          internal::c_time64_from_filetime(fd.ftCreationTime);
      fileinfo->time_access =
          internal::c_time64_from_filetime(fd.ftLastAccessTime);
      fileinfo->time_write =
          internal::c_time64_from_filetime(fd.ftLastWriteTime);
      fileinfo->size = size.QuadPart;
      wcsncpy(fileinfo->name, fd.cFileName, MAX_PATH);

      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
