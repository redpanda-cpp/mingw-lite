#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
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
  __DEFINE_THUNK(msvcrt,
                 0,
                 wchar_t *,
                 __cdecl,
                 _wfullpath,
                 wchar_t *absPath,
                 const wchar_t *relPath,
                 size_t maxLength)
  {
    if (internal::is_nt())
      return __ms__wfullpath(absPath, relPath, maxLength);

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
