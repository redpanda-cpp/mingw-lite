#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <io.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(intptr_t,
                     _wfindfirst32,
                     const wchar_t *filespec,
                     struct _wfinddata32_t *fileinfo)
  {
    if (internal::is_nt()) {
      static auto *pfn =
          internal::module_msvcrt.get_function<fn__wfindfirst32_t>(
              "_wfindfirst");
      return pfn(filespec, fileinfo);
    }

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
