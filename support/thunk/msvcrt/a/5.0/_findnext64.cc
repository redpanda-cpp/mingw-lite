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
    if (const auto pfn = try_get__findnext64())
      return pfn(handle, fileinfo);

    return impl::time32__findnext64(handle, fileinfo);
  }

  namespace impl
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
  } // namespace impl
} // namespace mingw_thunk
