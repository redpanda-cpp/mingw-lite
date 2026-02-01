#include "_wfindfirst64.h"

#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <io.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(msvcrt,
                 0,
                 intptr_t,
                 __cdecl,
                 _wfindfirst64,
                 const wchar_t *filespec,
                 struct _wfinddata64_t *fileinfo)
  {
    if (const auto pfn = try_get__wfindfirst64(); pfn && internal::is_nt())
      return pfn(filespec, fileinfo);

    return impl::time32__wfindfirst64(filespec, fileinfo);
  }

  namespace impl
  {
    intptr_t time32__wfindfirst64(const wchar_t *filespec,
                                  struct _wfinddata64_t *fileinfo)
    {
      WIN32_FIND_DATAW fd;
      HANDLE h = FindFirstFileW(filespec, &fd);
      if (h == INVALID_HANDLE_VALUE) {
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

      return reinterpret_cast<intptr_t>(h);
    }
  } // namespace impl
} // namespace mingw_thunk
