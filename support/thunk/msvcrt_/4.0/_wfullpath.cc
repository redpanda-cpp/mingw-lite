#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/wntcrt/errno.h>
#include <thunk/wntcrt/time.h>

#include <errno.h>
#include <stdlib.h>
#include <wchar.h>

#include <windows.h>

#undef _wfindnext

namespace mingw_thunk
{
  __DEFINE_CRT_THUNK(wchar_t *,
                     _wfullpath,
                     wchar_t *absPath,
                     const wchar_t *relPath,
                     size_t maxLength)
  {
    if (internal::is_nt())
      return get__wfullpath()(absPath, relPath, maxLength);

    wchar_t buffer[MAX_PATH];
    DWORD len = GetFullPathNameW(relPath, MAX_PATH, buffer, nullptr);
    if (len == 0) {
      internal::dosmaperr(GetLastError());
      return nullptr;
    } else if (len >= MAX_PATH) {
      _set_errno(ENAMETOOLONG);
      return nullptr;
    } else {
      if (len + 1 <= maxLength) {
        if (!absPath)
          absPath = (wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
        wmemcpy(absPath, buffer, len);
        absPath[len] = 0;
        return absPath;
      } else {
        _set_errno(ERANGE);
        return nullptr;
      }
    }
  }
} // namespace mingw_thunk
