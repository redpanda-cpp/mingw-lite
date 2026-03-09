#include "_findnext64.h"

#include <thunk/_common.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <io.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 int,
                 __cdecl,
                 _findnext64,
                 intptr_t handle,
                 struct __finddata64_t *fileinfo)
  {
    __DISPATCH_THUNK_2(_findnext64,
                       const auto pfn = try_get__findnext64(),
                       pfn,
                       &f::time32__findnext64);

    return dllimport__findnext64(handle, fileinfo);
  }

  namespace f
  {
    int time32__findnext64(intptr_t handle, struct __finddata64_t *fileinfo)
    {
      HANDLE h = reinterpret_cast<HANDLE>(handle);

      WIN32_FIND_DATAA fd;
      if (!FindNextFileA(h, &fd)) {
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
      strncpy(fileinfo->name, fd.cFileName, MAX_PATH);

      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
